#include "util.hpp"
#include "jasp_internal.hpp"

// TEMPORARY -- STREAM TEMPLATE IMAGE BYTE DATA TO FILE AND READ THAT INSTEAD
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <iostream>

namespace jasp {

	int init() {
		// Poll for RealSense devices until one is found
		rs2::context rs_ctx;
		auto devices = rs_ctx.query_devices();
		if (!devices.size()) {
			std::cout << "error: no RealSense device found!\n";
			return 0;
		}
		
		return 1;
	}

	bool start() {
		if (!internal::rs_pipe.poll_for_frames(&internal::rs_fset))
			return false;
		
		rs2::depth_frame rs_fdep = internal::rs_fset.get_depth_frame();
		internal::depth.data = const_cast<void*>(rs_fdep.get_data());
		rs2::video_frame rs_fcol = internal::rs_fset.get_color_frame();
		internal::color.data = const_cast<void*>(rs_fcol.get_data());
		
		/* thresholding */
		int width = internal::color.width;
		int height = internal::color.height;

		uint32_t* sum_table = new uint32_t[width * height];
		/* looks like this (example 5 x 5 grayscale image)
		 * 
		 * original:	summed:
		 * 1 1 1 1 1 	1  2  3  4  5
		 * 1 1 1 1 1	2  4  6  8  10
		 * 1 1 1 1 1 ->	3  6  9  12 15
		 * 1 1 1 1 1	4  4  12 16 20
		 * 1 1 1 1 1	5  10 15 20 25
		 * 
		 */
		
		for (int y = 0; y < height; ++y) {
			int sum = 0;
			for (int x = 0; x < width; ++x) {
				uint8_t* color_data = static_cast<uint8_t*>(internal::color.data) + (y * width + x) * internal::color.stride;
				uint8_t r = color_data[0];
				uint8_t g = color_data[1];
				uint8_t b = color_data[2];

				// generates grayscale/intensity value
				uint8_t intensity = 0.3f * r + 0.59f * g + 0.11f * b;

				uint8_t* thresh_data = static_cast<uint8_t*>(internal::threshold.data) + (y * width + x) * internal::threshold.stride;
				*thresh_data = intensity;

				// create summed area table
				sum += intensity;
				if (y == 0)
					sum_table[y * width + x] = sum;
				else
					sum_table[y * width + x] = sum + sum_table[(y - 1) * width + x];
			}
		}

		/* image segmentation */
		// adpatively thresholds each pixel by comparing it to an average of the surrounding pixels
		// source: https://people.scs.carleton.ca/~roth/iit-publications-iti/docs/gerh-50002.pdf
		uint32_t s = width / 8;
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				// defines the bounds for a window of the grayscale image
				int x1 = x - s / 2; x1 = x1 < 0 ? 0 : x1;
				int x2 = x + s / 2; x2 = x2 > width - 1 ? width - 1 : x2;
				int y1 = y - s / 2; y1 = y1 < 0 ? 0 : y1;
				int y2 = y + s / 2; y2 = y2 > height - 1 ? height - 1 : y2;
				uint32_t count = (x2 - x1) * (y2 - y1);

				// finds the average sum of intensity values of the grayscale image within the window bounds
				uint32_t top_left	= y1 - 1 < 0  || x1 - 1 < 0	? 0 : sum_table[(y1 - 1) * width + x1 - 1];
				uint32_t top_right	= y1 - 1 < 0				? 0 : sum_table[(y1 - 1) * width + x2];
				uint32_t bot_left	= x1 - 1 < 0				? 0 : sum_table[y2 * width + x1 - 1];
				uint32_t bot_right	= sum_table[y2 * width + x2];
				uint32_t sum = bot_right - bot_left - top_right + top_left;
				float avg = (float)sum / count;

				// the pixel at (x,y) must be at least (1 - t) percent of the average intensity to be white
				uint8_t* thresh_data = static_cast<uint8_t*>(internal::threshold.data) + (y * width + x) * internal::threshold.stride;
				*thresh_data = *thresh_data < avg * (1.0f - internal::ctx.threshold) ? 0x00 : 0xff;
			}
		}

		delete[] sum_table;

		/* detecting objects with template matching */



		/* configuration related */
		if (internal::ctx.config_flags & JASP_STREAM_DEPTH) {
			int width = internal::depth.width;
			int height = internal::depth.height;

			/* improve the visibility of the range of depth -- use realsense-viewer method? */
			unsigned int* pixel_data = new unsigned int[width * height];
			for (int y = 0; y < height; ++y) {
				for (int x = 0; x < width; ++x) {
					float d = rs_fdep.get_distance(x, y);
					d = (d >= 4) ? 4.0f : d;
					uint8_t v = (uint8_t)(d / 4.0f * 0xff);
					pixel_data[y * width + x] = 0xff000000 | (v << 16) | (v << 8) | (v << 0);
				}
			}

			if (internal::depth.pixel_data)
				delete[] internal::depth.pixel_data;
			internal::depth.pixel_data = pixel_data;
		}
	
		if (internal::ctx.config_flags & JASP_STREAM_COLOR) {
			int width = internal::color.width;
			int height = internal::color.height;

			unsigned int* pixel_data = new unsigned int[width * height];
			for (int y = 0; y < height; ++y) {
				for (int x = 0; x < width; ++x) {
					uint8_t* color = (uint8_t*)internal::color.data + (y * width + x) * internal::color.stride;
					uint8_t r = color[0];
					uint8_t g = color[1];
					uint8_t b = color[2];
					pixel_data[y * width + x] = 0xff000000 | (b << 16) | (g << 8) | (r << 0);
				}
			}

			for (const auto& target : internal::ctx.targets) {
				auto bounding_box = target->bbox;
				
				for (int x = bounding_box.x; x < bounding_box.x + bounding_box.w; ++x) {
					pixel_data[bounding_box.y * width + x] = 0xff00ffff;
					pixel_data[(bounding_box.y + bounding_box.h) * width + x] = 0xff00ffff;
				}

				for (int y = bounding_box.y; y < bounding_box.y + bounding_box.h; ++y) {
					pixel_data[y * width + bounding_box.x] = 0xff00ffff;
					pixel_data[y * width + bounding_box.x + bounding_box.w] = 0xff00ffff;
				}

			}

			if (internal::color.pixel_data)
				delete[] internal::color.pixel_data;
			internal::color.pixel_data = pixel_data;
		}
	

	
		if (internal::ctx.config_flags & JASP_STREAM_THRESHOLD) {
			int width = internal::threshold.width;
			int height = internal::threshold.height;

			unsigned int* pixel_data = new unsigned int[width * height];
			for (int y = 0; y < height; ++y) {
				for (int x = 0; x < width; ++x) {
					uint8_t intensity = *((uint8_t*)internal::threshold.data + (y * width + x) * internal::threshold.stride);
					pixel_data[y * width + x] = 0xff000000 | (intensity << 16) | (intensity << 8) | (intensity << 0);
				}
			}

			if (internal::threshold.pixel_data)
				delete[] internal::threshold.pixel_data;
			internal::threshold.pixel_data = pixel_data;
		}
	

		return true;
	}

	void shutdown() {
		return;
	}

}
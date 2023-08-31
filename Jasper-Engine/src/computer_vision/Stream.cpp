#include "Stream.hpp"
#include "../JaspInternal.hpp"

#include <cstring>

namespace Jasp {

	Stream::Stream() {
		width = 0;
		height = 0;
		stride = 0;
		data = nullptr;
		pixel_data = nullptr;
	}

	Stream::Stream(const Stream& other) {
		this->width = other.width;
		this->height = other.height;
		this->stride = other.stride;

		size_t n = this->width * this->height * this->stride;
		this->data = new uint8_t[n];
		std::memcpy(this->data, other.data, n);

		if (other.pixel_data) {
			n = this->width * this->height;
			this->pixel_data = new uint32_t[n];
			std::memcpy(this->pixel_data, other.pixel_data, n);
		}
		else
			this->pixel_data = nullptr;
	}

	Stream::~Stream() {
		if (pixel_data)
			delete[] pixel_data;
	}

    DepthStream::DepthStream() {
        rs2::depth_frame rs_fdep = Internal::rs_fset.get_depth_frame();
        width = rs_fdep.get_width();
        height = rs_fdep.get_height();
        stride = rs_fdep.get_bytes_per_pixel();
        data = const_cast<void*>(rs_fdep.get_data());
		pixel_data = nullptr;
    }

    DepthStream::DepthStream(const Jasp::DepthStream& other)
        :Stream(other) {
    }

	void DepthStream::RetrieveCameraData() {
		rs2::depth_frame rs_fdep = Internal::rs_fset.get_depth_frame();
		data = const_cast<void *>(rs_fdep.get_data());
	}

	void DepthStream::ConstructPixelData() {
		if (!pixel_data)
			pixel_data = new uint32_t[width * height];

		/* improve the visibility of the range of depth -- use realsense-viewer method? */
		rs2::depth_frame rs_fdep = Internal::rs_fset.get_depth_frame();
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				float d = rs_fdep.get_distance(x, y);
				d = (d >= 4) ? 4.0f : d;
				uint8_t v = (uint8_t)(d / 4.0f * 0xff);
				pixel_data[y * width + x] = 0xff000000 | (v << 16) | (v << 8) | (v << 0);
			}
		}
	}

	float DepthStream::GetDepthAtPixel(const Vec2& coord) const {
		rs2::depth_frame rs_fdep = Internal::rs_fset.get_depth_frame();
		return rs_fdep.get_distance(coord.x, coord.y);
	}

    ColorStream::ColorStream() {
		rs2::video_frame rs_fcol = Internal::rs_fset.get_color_frame();
		width = rs_fcol.get_width();
		height = rs_fcol.get_height();
		stride = rs_fcol.get_bytes_per_pixel();
		data = const_cast<void*>(rs_fcol.get_data());
		pixel_data = nullptr;
	}

    ColorStream::ColorStream(const Jasp::ColorStream& other)
        :Stream(other) {
    }

	void ColorStream::RetrieveCameraData() {
		rs2::video_frame rs_fcol = Internal::rs_fset.get_color_frame();
		data = const_cast<void *>(rs_fcol.get_data());
	}

	void ColorStream::ConstructPixelData() {
		if (!pixel_data)
			pixel_data = new uint32_t[width * height];

		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				uint8_t* color = (uint8_t*)data + (y * width + x) * stride;
				uint8_t r = color[0];
				uint8_t g = color[1];
				uint8_t b = color[2];
				pixel_data[y * width + x] = 0xff000000 | (b << 16) | (g << 8) | (r << 0);
			}
		}
	}

    ThreshStream::ThreshStream() {
		rs2::video_frame rs_fcol = Internal::rs_fset.get_color_frame();
		width = rs_fcol.get_width();
		height = rs_fcol.get_height();
		stride = 1;
		data = new uint8_t[width * height * stride];
		pixel_data = nullptr;
		threshold = 0.438f;
	}

    ThreshStream::ThreshStream(const Jasp::ThreshStream& other)
		:Stream(other) {
		threshold = other.threshold;
    }

	/*
	 * adpatively thresholds each pixel by comparing it to an average of the surrounding pixels
	 * source: https://people.scs.carleton.ca/~roth/iit-publications-iti/docs/gerh-50002.pdf
	 */
	void ThreshStream::Threshold(const ColorStream& color_stream) {
		uint32_t *sum_table = new uint32_t[width * height];
		for (int y = 0; y < height; ++y) {
			int sum = 0;
			for (int x = 0; x < width; ++x) {
				uint8_t* color_data = static_cast<uint8_t *>(color_stream.data) + (y * width + x) * color_stream.stride;
				uint8_t r = color_data[0];
				uint8_t g = color_data[1];
				uint8_t b = color_data[2];

				// generates grayscale/intensity value
				uint8_t intensity = 0.3f * r + 0.59f * g + 0.11f * b;

				uint8_t* thresh_data = static_cast<uint8_t*>(data) + (y * width + x);
				*thresh_data = intensity;

				// create summed area table
				sum += intensity;
				if (y == 0)
					sum_table[y * width + x] = sum;
				else
					sum_table[y * width + x] = sum + sum_table[(y - 1) * width + x];
			}
		}

		uint32_t s = width / 8;
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				// defines the bounds for a window of the grayscale image
				int x1 = x - s / 2;
				x1 = x1 < 0 ? 0 : x1;
				int x2 = x + s / 2;
				x2 = x2 > width - 1 ? width - 1 : x2;
				int y1 = y - s / 2;
				y1 = y1 < 0 ? 0 : y1;
				int y2 = y + s / 2;
				y2 = y2 > height - 1 ? height - 1 : y2;
				uint32_t count = (x2 - x1) * (y2 - y1);

				// finds the average sum of intensity values of the grayscale image within the window bounds
				uint32_t top_left = y1 - 1 < 0 || x1 - 1 < 0 ? 0 : sum_table[(y1 - 1) * width + x1 - 1];
				uint32_t top_right = y1 - 1 < 0 ? 0 : sum_table[(y1 - 1) * width + x2];
				uint32_t bot_left = x1 - 1 < 0 ? 0 : sum_table[y2 * width + x1 - 1];
				uint32_t bot_right = sum_table[y2 * width + x2];
				uint32_t sum = bot_right - bot_left - top_right + top_left;
				float avg = (float) sum / count;

				// the pixel at (x,y) must be at least (1 - t) percent of the average intensity to be white
				uint8_t* thresh_data = static_cast<uint8_t *>(data) + (y * width + x);
				*thresh_data = *thresh_data < avg * (1.0f - threshold) ? 0x00 : 0xff;
			}
		}

		delete[] sum_table;
	}

	void ThreshStream::RetrieveCameraData() {

	}

	void ThreshStream::ConstructPixelData() {
		if (!pixel_data)
			pixel_data = new uint32_t[width * height];

		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				uint8_t intensity = *((uint8_t*)data + (y * width + x) * stride);
				pixel_data[y * width + x] = 0xff000000 | (intensity << 16) | (intensity << 8) | (intensity << 0);
			}
		}
	}

}
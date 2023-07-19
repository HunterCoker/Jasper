#include "util.hpp"
#include "jasp_internal.hpp"

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
		
		
		// Depth
		{
			rs2::depth_frame rs_fdep = internal::rs_fset.get_depth_frame();
			internal::depth.data = const_cast<void*>(rs_fdep.get_data());
			if (internal::ctx.config_flags & JASP_STREAM_DEPTH) {
				int width = internal::depth.width;
				int height = internal::depth.height;

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
		}
		
		// Color
		{
			rs2::video_frame rs_fcol = internal::rs_fset.get_color_frame();
			internal::color.data = const_cast<void*>(rs_fcol.get_data());
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

				if (internal::color.pixel_data)
					delete[] internal::color.pixel_data;
				internal::color.pixel_data = pixel_data;
			}
		}

		/* threshold stream code... */

		/*..........................*/

		return true;
	}

	void shutdown() {
		return;
	}
};
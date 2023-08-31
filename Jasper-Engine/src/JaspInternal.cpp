#include "JaspInternal.hpp"

#include <iostream>
#include <unistd.h>

namespace Jasp {

	void Internal::Init() {
		// Poll for RealSense devices until one is found
		rs2::context rs_ctx;
		while (true) {
			auto devices = rs_ctx.query_devices();
			if (!devices.size()) {
				std::cout << "error: no RealSense device found!\n";
				sleep(3);
				continue;
			}
			break;
		}

		// enables RealSense Camera streams and starts them
		Internal::rs_cfg.enable_stream(RS2_STREAM_DEPTH, -1, 848, 480, RS2_FORMAT_ANY, 60);
		Internal::rs_cfg.enable_stream(RS2_STREAM_COLOR, -1, 848, 480, RS2_FORMAT_ANY, 60);
		Internal::rs_pipe.start(Internal::rs_cfg);
		Internal::rs_fset = Internal::rs_pipe.wait_for_frames();
	}

}

#pragma once

#include "context.hpp"
#include "stream.hpp"
#include "detection.hpp"

#include <librealsense2/rs.hpp>

namespace jasp {

	namespace internal {
		
		/* global objects */
		inline context ctx;

		inline stream depth;
		inline stream color;
		inline stream threshold;
		
		inline target_detector detector;

		inline rs2::config rs_cfg;
		inline rs2::pipeline rs_pipe;
		inline rs2::frameset rs_fset;

	}

}

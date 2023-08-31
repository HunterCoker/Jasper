#pragma once

#include "computer_vision/Stream.hpp"
#include "computer_vision/Detection.hpp"

#include <vector>

namespace Jasp {

	struct JaspContext {
        bool debug = false;

		/* Computer Vision */
		TargetDetector detector;
		TargetList targets;
        DepthStream depth;
		ColorStream color;
		ThreshStream threshold;

		/* Aimbot System */
		// ...
	};

}
#pragma once

enum config_flags {
	JASP_STREAM_ALL			= 0x7,
	JASP_STREAM_DEPTH		= 0x1,
	JASP_STREAM_COLOR		= 0x2,
	JASP_STREAM_THRESHOLD	= 0x4
};

namespace Jasp {

	struct Vec2 {
		int x, y;
	};

	struct Rect {
		int x, y;
		int w, h;
	};

}
#pragma once

#include "util.hpp"

namespace jasp {
	
	struct rectbb {
		int x, y;
		int w, h;
	};

	struct target {
		rectbb bbox;
		point c;
	};

}
#pragma once

#include "util.hpp"

namespace jasp {

	struct target {
		rectbb bbox;
		point c;
		const char* name;
	};

}
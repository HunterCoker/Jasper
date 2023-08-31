#pragma once

#include "../Util.hpp"

#include <vector>

namespace Jasp {

	struct Target {
		Rect bbox;
		Vec2 c;
	};

	typedef std::vector<Target> TargetList;

}
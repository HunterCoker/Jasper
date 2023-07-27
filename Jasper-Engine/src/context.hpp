#pragma once

#include "stream.hpp"
#include "target.hpp"

#include <vector>

namespace jasp {

	struct context {
		context() {
			config_flags = 0;
			threshold = 0.483f;
		}

		std::vector<stream*> streams;
		std::vector<target*> targets;
		unsigned int config_flags;
		float threshold;
	};

	void make_context_current(const context& ctx);
	context* get_current_context();

}
#pragma once

#include "stream.hpp"
#include "target.hpp"

#include <vector>

namespace jasp {

	struct context {
		unsigned int config_flags;
		std::vector<stream*> streams;
		std::vector<target*> targets;
	};

	void make_context_current(const context& ctx);
	const context& get_current_context();

}
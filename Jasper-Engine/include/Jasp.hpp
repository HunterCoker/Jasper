#pragma once

#include "../src/Util.hpp"
#include "../src/Context.hpp"
#include "../src/computer_vision/Stream.hpp"
#include "../src/computer_vision/Detection.hpp"

namespace Jasp {

	// API
    JaspContext* CreateContext();
    JaspContext* GetCurrentContext();
    bool NewFrame();
	void Terminate();
	[[nodiscard]] TargetList& GetTargets();

	// Debug API
	[[nodiscard]] DepthStream& GetDepthStream();
	[[nodiscard]] ColorStream& GetColorStream();
	[[nodiscard]] ThreshStream& GetThreshStream();

}
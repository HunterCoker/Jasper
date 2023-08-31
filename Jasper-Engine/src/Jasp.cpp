#include "Jasp.hpp"

#include "JaspInternal.hpp"

Jasp::JaspContext* JaspContext_g = nullptr;

Jasp::JaspContext* Jasp::CreateContext() {
	Internal::Init();

    if (!JaspContext_g)
        JaspContext_g = new Jasp::JaspContext;
    else {
        delete JaspContext_g;
        JaspContext_g = new Jasp::JaspContext;
    }

	return JaspContext_g;
}

Jasp::JaspContext* Jasp::GetCurrentContext() {
    return JaspContext_g;
}

bool Jasp::NewFrame() {
	Jasp::JaspContext* g = Jasp::GetCurrentContext();

	/*
	 * Retrieves data every time a frame is captured by the RealSense camera
	 * and converts it into usable data for the computer vision model.
	 */
	if (Internal::rs_pipe.poll_for_frames(&Internal::rs_fset)) {
		g->depth.RetrieveCameraData();
		g->color.RetrieveCameraData();
		g->threshold.Threshold(g->color);

		if (g->debug) {
			g->depth.ConstructPixelData();
			g->color.ConstructPixelData();
			g->threshold.ConstructPixelData();
		}
	}

	return true;
}

Jasp::TargetList& Jasp::GetTargets() {
	Jasp::JaspContext* g = Jasp::GetCurrentContext();
	return g->targets;
}

void Jasp::Terminate() {
	delete JaspContext_g;
}

Jasp::DepthStream& Jasp::GetDepthStream() {
	Jasp::JaspContext* g = Jasp::GetCurrentContext();
	return g->depth;
}
Jasp::ColorStream& Jasp::GetColorStream() {
	Jasp::JaspContext* g = Jasp::GetCurrentContext();
	return g->color;
}
Jasp::ThreshStream& Jasp::GetThreshStream() {
	Jasp::JaspContext* g = Jasp::GetCurrentContext();
	return g->threshold;
}
#include "context.hpp"
#include "jasp_internal.hpp"

namespace jasp {

	void make_context_current(const context& ctx) {
		internal::ctx = ctx;

		// enables required RealSense Camera streams and starts them
		internal::rs_cfg.enable_stream(RS2_STREAM_DEPTH, -1, 848, 480, RS2_FORMAT_ANY, 60);
		internal::rs_cfg.enable_stream(RS2_STREAM_COLOR, -1, 848, 480, RS2_FORMAT_ANY, 60);
		internal::rs_pipe.start(internal::rs_cfg);
		internal::rs_fset = internal::rs_pipe.wait_for_frames();

		// initializes global jasp::stream objects for image processing & rendering (if configured)
		stream::create_depth_stream(internal::depth);
		if (ctx.config_flags & JASP_STREAM_DEPTH) {
			internal::ctx.streams.push_back(&internal::depth);
		}

		stream::create_color_stream(internal::color);
		if (ctx.config_flags & JASP_STREAM_COLOR) {
			internal::ctx.streams.push_back(&internal::color);
		}
		
		stream::create_threshold_stream(internal::threshold);
		if (ctx.config_flags & JASP_STREAM_THRESHOLD) {
			internal::ctx.streams.push_back(&internal::threshold);
		}
		
		// create thread for managing internal streams & detecting targets
	}

	const context& get_current_context() {
		return internal::ctx;
	}

}
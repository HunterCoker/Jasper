#include "Application.hpp"

#include <iostream>

Application* Application::instance_ = nullptr;

void Application::Run() {
	if (!Init())
		throw std::runtime_error("could not initialize application");
	
	while (app_should_run_) {
		HandleEvents();
		Update();
		Render();
	}

	Terminate();
}

bool Application::Init() {

    /* take a snapshot and retrieve depth data */
	config_ = new rs2::config();
	config_->enable_stream(RS2_STREAM_DEPTH, RS2_FORMAT_ANY);

    pipeline_ = new rs2::pipeline();
    pipeline_->start(*config_);

	frameset_ = new rs2::frameset();
	*frameset_ = pipeline_->wait_for_frames(3000);
	rs2::depth_frame init_depth_frame = frameset_->first(RS2_STREAM_DEPTH);

	if (SDL_Init(SDL_INIT_EVENTS) < 0) {
		SDL_Log("error: failed to initialize SDL: %s", SDL_GetError());
		return false;
	}

	window_ = SDL_CreateWindow(
		"Jasper",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		init_depth_frame.get_width(),
		init_depth_frame.get_height(),
		SDL_WINDOW_SHOWN);
	if (!window_) {
		SDL_Log("error: failed to create window: %s", SDL_GetError());
		return false;
	}
	surface_ = SDL_GetWindowSurface(window_);

	return true;
}

void Application::HandleEvents() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			app_should_run_ = false;
			break;
		default:
			break;
		}
	}
}

void Application::Update() {
	frame_++;

	SDL_LockSurface(surface_);
	
	*frameset_ = pipeline_->wait_for_frames(3000);
    rs2::depth_frame depth_frame = frameset_->first(RS2_STREAM_DEPTH);
	int width = depth_frame.get_width();
	int height = depth_frame.get_height();

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			float d = depth_frame.get_distance(x, y);
			d = (d >= 4) ? 4.0f : d;

			/* for rendering */
			uint8_t v = (uint8_t)(d / 4.0f * 0xff);
			uint32_t* target_pixel = (uint32_t*)surface_->pixels + (y * width + x);
			*target_pixel = 0xff000000 | (v << 16) | (v << 8) | (v << 0);

			/* object detection */

		}
	}
	
	SDL_UnlockSurface(surface_);

}

void Application::Render() {
	SDL_UpdateWindowSurface(window_);
}

void Application::Terminate() {
	SDL_DestroyWindow(window_);
	SDL_FreeSurface(surface_);
	SDL_Quit();
}
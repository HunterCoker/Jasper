#pragma once

#include <SDL2/SDL.h>
#include <librealsense2/rs.hpp>

class Application {
public:
	void Run();
	static Application* GetInstance() { 
		if (!instance_)
			instance_ = new Application();
		return instance_;
	}
private:
	Application() = default;
	~Application() = default;
	static Application* instance_;

	/* main runtime loop */
	bool Init();
	void HandleEvents();
	void Update();
	void Render();
	void Terminate();
private:
	unsigned int frame_ = 0;
	bool app_should_run_ = true;
private:
	/* SDL specific objects */
	SDL_Window* window_ = nullptr;
	SDL_Surface* surface_ = nullptr;
	/* RealSense SDK 2.0 specific objects */
	rs2::config* config_ = nullptr;
	rs2::pipeline* pipeline_ = nullptr;
	rs2::frameset* frameset_ = nullptr;
};
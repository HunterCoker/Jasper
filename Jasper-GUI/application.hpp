#pragma once

#include <jasp.hpp>

#include <SDL.h>

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

#include <iostream>
#include <vector>

class Application {
public:
	Application();
	Application(const Application&) = delete;
	~Application();

	void Run();
private:
	SDL_Window* window_;
	SDL_Renderer* renderer_;
	std::vector<SDL_Texture*> textures_;
};
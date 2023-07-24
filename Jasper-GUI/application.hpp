#pragma once

#include <jasp.hpp>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

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
#include "application.hpp"

#include <imgui_internal.h>

Application::Application() {
	// Setup Jasp Engine with context
	if (!jasp::init()) {
		std::exit(-1);
	}
	jasp::context ctx;
	ctx.config_flags = JASP_STREAM_COLOR | JASP_STREAM_THRESHOLD;
	jasp::make_context_current(ctx);

	// Setup SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMEPAD) != 0) {
		printf("Error: SDL_Init(): %s\n", SDL_GetError());
		std::exit(-1);
	}

	// Enable native IME.
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

	// Create SDL_Window with SDL_Renderer graphics context
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
	window_ = SDL_CreateWindow("Jasper GUI", 1280, 720, window_flags);
	if (window_ == nullptr) {
		printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		std::exit(-1);
	}
	renderer_ = SDL_CreateRenderer(window_, NULL, SDL_RENDERER_ACCELERATED);
	if (renderer_ == nullptr) {
		SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
		std::exit(-1);
	}
	SDL_SetWindowPosition(window_, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_ShowWindow(window_);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style and Dockspace
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL3_InitForSDLRenderer(window_, renderer_);
	ImGui_ImplSDLRenderer3_Init(renderer_);
}

void Application::Run() {
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Docking

	jasp::context* ctx = jasp::get_current_context();

	// Create SDL_Textures that correspond to each stream created by the given context
	textures_.resize(ctx->streams.size());
	for (std::size_t i = 0; i < textures_.size(); ++i) {
		auto stream = ctx->streams[i];
		textures_[i] = SDL_CreateTexture(renderer_,
			SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING,
			stream->width, stream->height);
	}

	bool running = true;
	while (running) {

		// Handle events
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT)
				running = false;
			if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window_))
				running = false;
		}

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		static bool init_dockspace = false;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(viewport);

		if (!init_dockspace) {
			ImGui::DockBuilderRemoveNode(dockspace_id);
			ImGui::DockBuilderAddNode(dockspace_id);
			
			ImGuiID dockspace_id_left, dockspace_id_right;
			ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.3f, &dockspace_id_left, &dockspace_id_right);
			ImGui::DockBuilderDockWindow("Debug", dockspace_id_left);
			ImGui::DockBuilderDockWindow("Viewport", dockspace_id_right);

			ImGui::DockBuilderFinish(dockspace_id);
			init_dockspace = true;
		}

		// Start a new Jasp Engine iteration
		bool itr = jasp::start();
		
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoBackground | ImGuiDockNodeFlags_PassthruCentralNode |
			ImGuiWindowFlags_NoMove;

		// Debug window
		static bool paused = false;
		{
			ImGui::Begin("Debug", nullptr, window_flags);

			ImGui::Text("General");
			if (ImGui::Button("Pause")) {
				if (paused) {
					paused = false;
				}
				else {
					paused = true;
				}
			}

			if (ImGui::Button("Capture")) {
				for (std::size_t i = 0; i < ctx->streams.size(); ++i) {
					jasp::stream* stream = ctx->streams[i];

					// capture each stream's pixel data as a png with appropriate filenames
					// stbi_write_png("...", stream->width, stream->height, 4, stream->pixel_data, 4 * stream->width);
				}
			}
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

			ImGui::NewLine();
			ImGui::Text("Stream");
			ImGui::SliderFloat("threshold", &ctx->threshold, 0.0f, 1.0f, "%.3f", 0);

			ImGui::End();
		}

		// Viewport window
		{
			ImGui::Begin("Viewport", nullptr, window_flags);
			
			auto streams = ctx->streams;
			// auto targets = ctx->targets;

			ImVec2 window_size = ImGui::GetWindowSize();
			for (std::size_t i = 0; i < streams.size(); ++i) {

				auto stream = streams[i];
				if (itr && !paused) {
					SDL_UpdateTexture(textures_[i], nullptr, stream->pixel_data, 4 * stream->width);
				}

				float aspect = (float)stream->height / stream->width;
				ImVec2 image_size = { window_size.x , window_size.x * aspect };
				// ImVec2 image_size = ...;
				// ImVec2 local_pos = ...;
				// ImGui::SetCursorPos(local_pos);
				// ImGui::Image((ImTextureID)(intptr_t)textures_[i], image_size);
				ImGui::Image((ImTextureID)(intptr_t)textures_[i], image_size);
			}

			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		//SDL_RenderSetScale(rende	rer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		SDL_SetRenderDrawColor(renderer_, 0x73, 0x8c, 0x99, 0xff);
		SDL_RenderClear(renderer_);
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(renderer_);
	}
}

Application::~Application() {
	jasp::shutdown();

	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	SDL_Quit();
}
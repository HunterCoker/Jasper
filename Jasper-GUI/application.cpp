#include "application.hpp"

Application::Application() {
	// Setup Jasp Engine with context
	if (!jasp::init()) {
		std::exit(-1);
	}
	jasp::context ctx;
	ctx.config_flags = JASP_STREAM_COLOR | JASP_STREAM_DEPTH;
	jasp::make_context_current(ctx);

	// Setup SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
		printf("Error: %s\n", SDL_GetError());
		std::exit(-1);
	}

	// Create SDL_Window with SDL_Renderer graphics context
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	window_ = SDL_CreateWindow("Jasper-GUI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
	renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
	if (renderer_ == nullptr) {
		SDL_Log("Error creating SDL_Renderer!");
		std::exit(-1);
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(window_, renderer_);
	ImGui_ImplSDLRenderer2_Init(renderer_);
}

void Application::Run() {
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Docking

	const jasp::context ctx = jasp::get_current_context();

	// Create SDL_Textures that correspond to each stream created by the given context
	textures_.resize(ctx.streams.size());
	for (std::size_t i = 0; i < textures_.size(); ++i) {
		auto stream = ctx.streams[i];
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
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				running = false;
			if (event.type == SDL_WINDOWEVENT &&
				event.window.event == SDL_WINDOWEVENT_CLOSE &&
				event.window.windowID == SDL_GetWindowID(window_))
				running = false;
		}

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		// Start a new Jasp Engine iteration
		bool itr = jasp::start();

		// 1. Create dockspace
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::DockSpaceOverViewport(viewport);

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoBackground | ImGuiDockNodeFlags_PassthruCentralNode |
			ImGuiWindowFlags_NoMove;

		// Debug window
		static bool paused = false;
		{
			ImGui::Begin("Debug", nullptr, window_flags);
			ImGui::Text("Image information:");

			if (ImGui::Button("Pause")) {
				if (paused) {
					paused = false;
				}
				else {
					paused = true;
				}
			}

			ImGui::NewLine();
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}

		// Viewport window
		{
			ImGui::Begin("Viewport", nullptr, window_flags);
			
			auto streams = ctx.streams;
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
		SDL_RenderSetScale(renderer_, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		SDL_SetRenderDrawColor(renderer_, 0x73, 0x8c, 0x99, 0xff);
		SDL_RenderClear(renderer_);
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(renderer_);
	}
}

Application::~Application() {
	jasp::shutdown();

    ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	SDL_Quit();
}
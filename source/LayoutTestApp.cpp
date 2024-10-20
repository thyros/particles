#include "LayoutTestApp.h"

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>
#include <imgui.h>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

std::unique_ptr<IApp> CreateLayoutTestApp(int16_t width, int16_t height) {
	
	// ####################################
	// ## SDL
    // ####################################

    printf("CreateLayoutTestApp\n");

    if (!SDL_Init(SDL_INIT_VIDEO))
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return nullptr;
	}

	constexpr int windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
	SDL_Window *window = SDL_CreateWindow("SDL Tutorial", width, height, windowFlags);
	if (window == nullptr)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return nullptr;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
	if (renderer == nullptr)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return nullptr;
	}

	// ####################################
	// ## IMGUI
    // ####################################

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsDark();

	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);

	return std::make_unique<LayoutTestApp>(width, height, window, renderer);
}

LayoutTestApp::LayoutTestApp(int16_t width, int16_t height, SDL_Window *window, SDL_Renderer *renderer) :
    mWidth(width),
    mHeight(height),
    mWindow(window),
    mRenderer(renderer)
{}

LayoutTestApp::~LayoutTestApp()
{
    // ####################################
	// ## IMGUI
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	// ####################################
	// ## SDL
	// SDL_DestroyTexture(mSpriteTexture);
	// mSpriteTexture = nullptr;

	// Deallocate surface
	// SDL_DestroySurface(mSurface);
	// mSurface = nullptr;

	SDL_DestroyRenderer(mRenderer);
	mRenderer = nullptr;

	// Destroy window
	SDL_DestroyWindow(mWindow);
	mWindow = nullptr;

	// Quit SDL subsystems
	SDL_Quit();
}

void LayoutTestApp::Run() {
    bool quit = false;

    while (!quit) {
        quit = Update();
        Render();
    }
}

bool LayoutTestApp::Update() {

	const ImGuiIO &io = ImGui::GetIO();

	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		ImGui_ImplSDL3_ProcessEvent(&e);
		if (io.WantCaptureMouse)
		{
			continue;
		}
		if (e.type == SDL_EVENT_QUIT)
		{
			return true;
		}
    }


    return false;
}

void LayoutTestApp::Render() {

	constexpr ImVec4 clearColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	SDL_SetRenderDrawColor(mRenderer, (Uint8)(clearColor.x * 255), (Uint8)(clearColor.y * 255), (Uint8)(clearColor.z * 255), (Uint8)(clearColor.w * 255));
	SDL_RenderClear(mRenderer);

	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport();

    ImGui::Begin("Game");
    ImGui::Text("Game");
    ImGui::End();

    ImGui::Begin("Config");
    ImGui::Text("Config");
    ImGui::End();

	ImGui::Render();

	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), mRenderer);
	SDL_RenderPresent(mRenderer);
}
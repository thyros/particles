#include "LayoutTestApp.h"

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>
#include <imgui.h>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_image.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

namespace {
	
/// @brief Draws a white frame of a desired width in a provided renderer.
void DrawFrame(SDL_Renderer *renderer, float w = 3)
{
    /* Get the Size of drawing surface */
    SDL_Rect darea;
    SDL_GetRenderViewport(renderer, &darea);

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	const SDL_FRect top{0, 0, (float)darea.w, w};
	const SDL_FRect left{0, 0, w, (float)darea.h};
	const SDL_FRect bottom{0, (float)(darea.h - w), (float)darea.w, w};
	const SDL_FRect right{(float)(darea.w - w), 0, w, (float)darea.h};
	
	SDL_RenderFillRect(renderer, &top);
	SDL_RenderFillRect(renderer, &left);
	SDL_RenderFillRect(renderer, &bottom);
	SDL_RenderFillRect(renderer, &right);
}

/// @brief Scales the textureSize to fit the availableSpace and conserving the original aspect ratio
ImVec2 ScaleToFit(ImVec2 textureSize, ImVec2 availableSpace) {
	const float ratio = std::min(availableSpace.x / textureSize.x, availableSpace.y / textureSize.y);
	return ImVec2{textureSize.x * ratio, textureSize.y * ratio};
}
}

std::unique_ptr<IApp> CreateLayoutTestApp(int16_t width, int16_t height) {
    // ####################################
    // ## SDL
    // ####################################

    printf("CreateLayoutTestApp\n");

    if(!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return nullptr;
    }

    constexpr int windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    SDL_Window *window = SDL_CreateWindow("SDL Tutorial", width, height, windowFlags);
    if(window == nullptr) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return nullptr;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    if(renderer == nullptr) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        return nullptr;
    }

    SDL_Surface *surface = IMG_Load("res/circle.png");
    if(surface == nullptr) {
        printf("Could not load image");
        return nullptr;
    }
    SDL_Texture *spriteTexture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Texture *backBuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888,
                                                SDL_TEXTUREACCESS_TARGET, width, height);

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

    return std::make_unique<LayoutTestApp>(width, height, window, renderer, surface, spriteTexture, backBuffer);
}

LayoutTestApp::LayoutTestApp(int16_t width, int16_t height, SDL_Window *window, SDL_Renderer *renderer, SDL_Surface *surface,
                             SDL_Texture *spriteTexture, SDL_Texture *backBuffer)
    : mWidth(width),
      mHeight(height),
      mWindow(window),
      mRenderer(renderer),
      mSurface(surface),
      mSpriteTexture(spriteTexture),
      mBackBuffer(backBuffer) {}

LayoutTestApp::~LayoutTestApp() {
    // ####################################
    // ## IMGUI
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    // ####################################
    // ## SDL
    SDL_DestroyTexture(mBackBuffer);
    mBackBuffer = nullptr;

    SDL_DestroyTexture(mSpriteTexture);
    mSpriteTexture = nullptr;

    // Deallocate surface
    SDL_DestroySurface(mSurface);
    mSurface = nullptr;

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

    while(!quit) {
        quit = Update();
        Render();
    }
}

bool LayoutTestApp::Update() {
    const ImGuiIO &io = ImGui::GetIO();

    SDL_Event e;
    while(SDL_PollEvent(&e) != 0) {
        ImGui_ImplSDL3_ProcessEvent(&e);
        if(io.WantCaptureMouse) {
            continue;
        }
        if(e.type == SDL_EVENT_QUIT) {
            return true;
        }
    }

    return false;
}

void LayoutTestApp::Render() {
    SDL_SetRenderTarget(mRenderer, mBackBuffer);

    constexpr ImVec4 clearColor = ImVec4(0.0f, 0.2f, 0.0f, 1.0f);
    SDL_SetRenderDrawColor(mRenderer, (Uint8)(clearColor.x * 255), (Uint8)(clearColor.y * 255), (Uint8)(clearColor.z * 255), (Uint8)(clearColor.w * 255));
    SDL_RenderClear(mRenderer);

	DrawFrame(mRenderer);

    const int size = 100;
    const int x = 100;
    const int y = 100;
    const float offset = size / 2.0f;

    SDL_SetTextureColorMod(mSpriteTexture, static_cast<char>(255 * 1.0f),
                           static_cast<char>(255 * 0.5f),
                           static_cast<char>(255 * 0.0f));

    SDL_FRect dst{.x = x - offset,
                  .y = y - offset,
                  .w = static_cast<float>(size),
                  .h = static_cast<float>(size)};

    SDL_RenderTexture(mRenderer, mSpriteTexture, nullptr, &dst);

    SDL_SetRenderTarget(mRenderer, nullptr);
    constexpr ImVec4 blackColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    SDL_SetRenderDrawColor(mRenderer, (Uint8)(blackColor.x * 255), (Uint8)(blackColor.y * 255), (Uint8)(blackColor.z * 255), (Uint8)(blackColor.w * 255));
    SDL_RenderClear(mRenderer);

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport();

	const ImGuiStyle& style = ImGui::GetStyle();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Game");
	const float titlebarHeight = 2 * style.FramePadding.y + ImGui::GetFontSize();
	const ImVec2 windowSize = ImGui::GetWindowSize();
	const ImVec2 availableSpace = ImVec2{windowSize.x, windowSize.y - titlebarHeight};
	const ImVec2 originalTextureSize = ImVec2(mWidth, mHeight);
	const ImVec2 desiredTextureSize = ScaleToFit(originalTextureSize, availableSpace);

	const ImVec2 textureOffset = ImVec2{(availableSpace.x - desiredTextureSize.x) / 2, (availableSpace.y - desiredTextureSize.y) / 2};
	ImGui::SetCursorPos(ImVec2{textureOffset.x, textureOffset.y + titlebarHeight});
    ImGui::Image((ImTextureID)(intptr_t)mBackBuffer, desiredTextureSize);

	ImGui::End();
	ImGui::PopStyleVar();

    ImGui::Begin("Config");
	ImGui::Text("windowSize %f %f", windowSize.x, windowSize.y);
	ImGui::Text("availableSpace %f %f", availableSpace.x, availableSpace.y);
	ImGui::Text("originalTextureSize %f %f", originalTextureSize.x, originalTextureSize.y);
	ImGui::Text("desiredTextureSize %f %f", desiredTextureSize.x, desiredTextureSize.y);
	ImGui::Text("offset %f %f", textureOffset.x, textureOffset.y);
    ImGui::End();

    ImGui::Render();

    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), mRenderer);
    SDL_RenderPresent(mRenderer);
}

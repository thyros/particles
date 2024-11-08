#include "ConfigFunctions.h"
#include "LayoutTestApp.h"


#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>
#include <imgui.h>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_image.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include <string>

namespace {

/// @brief Draws a white frame of a desired width in a provided renderer.
void DrawFrame(SDL_Renderer *renderer, float w = 3) {
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

/// @brief Renders
void RenderTexture(SDL_Renderer *renderer, SDL_Texture *texture, float x, float y, int size, float r, float g, float b) {
    const float offset = size / 2;
    SDL_FRect dst{.x = x - offset,
                  .y = y - offset,
                  .w = static_cast<float>(size),
                  .h = static_cast<float>(size)};

    SDL_SetTextureColorMod(texture, static_cast<char>(255 * r),
                           static_cast<char>(255 * g),
                           static_cast<char>(255 * b));
    SDL_RenderTexture(renderer, texture, nullptr, &dst);
}

/// @brief Scales the textureSize to fit the availableSpace and conserving the original aspect ratio
ImVec2 ScaleToFit(ImVec2 textureSize, ImVec2 availableSpace) {
    const float ratio = std::min(availableSpace.x / textureSize.x, availableSpace.y / textureSize.y);
    return ImVec2{textureSize.x * ratio, textureSize.y * ratio};
}
} // namespace

std::unique_ptr<IApp> CreateLayoutTestApp(Config &config, State &state, int16_t width, int16_t height) {
    // ####################################
    // ## SDL
    // ####################################

    printf("CreateLayoutTestApp\n");

    if(!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return nullptr;
    }

    constexpr int windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    SDL_Window_Handle window(SDL_CreateWindow("SDL Tutorial", width, height, windowFlags), SDL_DestroyWindow);
    if(window == nullptr) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return nullptr;
    }

    SDL_Renderer_Handle renderer(SDL_CreateRenderer(window.get(), nullptr), SDL_DestroyRenderer);
    if(renderer == nullptr) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        return nullptr;
    }

    SDL_Surface_Handle surface(IMG_Load("res/circle.png"), SDL_DestroySurface);
    if(surface == nullptr) {
        printf("Could not load image");
        return nullptr;
    }
    SDL_Texture_Handle spriteTexture(SDL_CreateTextureFromSurface(renderer.get(), surface.get()), SDL_DestroyTexture);
    SDL_Texture_Handle backBuffer(SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, width, height), SDL_DestroyTexture);

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

    ImGui_ImplSDL3_InitForSDLRenderer(window.get(), renderer.get());
    ImGui_ImplSDLRenderer3_Init(renderer.get());

    return std::make_unique<LayoutTestApp>(config, state, width, height, std::move(window), std::move(renderer), std::move(surface), std::move(spriteTexture), std::move(backBuffer));
}

LayoutTestApp::LayoutTestApp(Config &config, State &state, int16_t width, int16_t height, SDL_Window_Handle window, SDL_Renderer_Handle renderer, SDL_Surface_Handle surface,
                             SDL_Texture_Handle spriteTexture, SDL_Texture_Handle backBuffer)
    : mConfig(config), mState(state), mWidth(width),
      mHeight(height),
      mWindow(std::move(window)),
      mRenderer(std::move(renderer)),
      mSurface(std::move(surface)),
      mSpriteTexture(std::move(spriteTexture)),
      mBackBuffer(std::move(backBuffer)) {

    
}

LayoutTestApp::~LayoutTestApp() {
    // ####################################
    // ## IMGUI
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    // ####################################
    // ## SDL
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
    const ImVec2 mousePos = io.MousePos;
    if (mousePos.x >= mGameTopLeft.x && mousePos.y >= mGameTopLeft.y && mousePos.x < mGameTopLeft.x + mGameSize.x && mousePos.y < mGameTopLeft.y + mGameSize.y) {
        const Position factor{mWidth/mGameSize.x, mHeight/mGameSize.y};        
        mGamePosition.x = (mousePos.x - mGameTopLeft.x) * factor.x;
        mGamePosition.y = (mousePos.y - mGameTopLeft.y) * factor.y;
    } else {
        mGamePosition.x = -1;
        mGamePosition.y = -1;
    }

    SDL_Event e;
    while(SDL_PollEvent(&e) != 0) {
        ImGui_ImplSDL3_ProcessEvent(&e);
        // if(io.WantCaptureMouse) {
        //     continue;
        // }

        if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == 1) {
            if (mGamePosition.x != -1 && mGamePosition.y != -1) {
                const int c = 1;
                AddParticle(mState, mGamePosition.x, mGamePosition.y, c);            
            } 
        }

        if(e.type == SDL_EVENT_QUIT) {
            return true;
        }
    }
    return false;
}

void LayoutTestApp::Render() {
    SDL_SetRenderTarget(mRenderer.get(), mBackBuffer.get());

    constexpr ImVec4 blackColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    SDL_SetRenderDrawColor(mRenderer.get(), (Uint8)(blackColor.x * 255), (Uint8)(blackColor.y * 255), (Uint8)(blackColor.z * 255), (Uint8)(blackColor.w * 255));
    SDL_RenderClear(mRenderer.get());

    RenderParticles();

    SDL_SetRenderTarget(mRenderer.get(), nullptr);
    SDL_SetRenderDrawColor(mRenderer.get(), (Uint8)(blackColor.x * 255), (Uint8)(blackColor.y * 255), (Uint8)(blackColor.z * 255), (Uint8)(blackColor.w * 255));
    SDL_RenderClear(mRenderer.get());

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport();

    const ImGuiStyle &style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Game");
    const float titlebarHeight = 2 * style.FramePadding.y + ImGui::GetFontSize();
    const ImVec2 windowSize = ImGui::GetWindowSize();
    const ImVec2 availableSpace = ImVec2{windowSize.x, windowSize.y - titlebarHeight};
    const ImVec2 originalTextureSize = ImVec2(mWidth, mHeight);
    const ImVec2 desiredTextureSize = ScaleToFit(originalTextureSize, availableSpace);

    const ImVec2 textureOffset = ImVec2{(availableSpace.x - desiredTextureSize.x) / 2, (availableSpace.y - desiredTextureSize.y) / 2};
    ImGui::SetCursorPos(ImVec2{textureOffset.x, textureOffset.y + titlebarHeight});
    ImGui::Image((ImTextureID)(intptr_t)mBackBuffer.get(), desiredTextureSize);

    mGameTopLeft.x = textureOffset.x;
    mGameTopLeft.y = textureOffset.y + titlebarHeight;
    mGameSize.x = desiredTextureSize.x;
    mGameSize.y = desiredTextureSize.y;

    ImGui::End();
    ImGui::PopStyleVar();

    ImGui::Begin("Utils");
    // RenderConfig(mConfig);
    RenderDebugInfo();
    ImGui::End();

    ImGui::Render();

    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), mRenderer.get());
    SDL_RenderPresent(mRenderer.get());
}

void LayoutTestApp::RenderConfig(Config &config) {
    int currentColor = 3;
    constexpr ImVec2 colorBoxSize(25.0f, 25.0f);

    ImGui::Text("Current Color: %i",
                currentColor + 1); // Display some text (you can use a format strings too)

    // Particle colors
    for(size_t i = 0; i < config.colorsCount; ++i) {
        ImGuiColorEditFlags misc_flags = 0;
        float color[3]{config.particleColors[i].r, config.particleColors[i].g,
                       config.particleColors[i].b};
        std::string id = "Color " + std::to_string(i + 1);
        ImGui::ColorEdit3(id.c_str(), (float *)&color, misc_flags);
        config.particleColors[i].r = color[0];
        config.particleColors[i].g = color[1];
        config.particleColors[i].b = color[2];
    }

    // Matrix
    for(int x = 0; x < config.colorsCount; ++x) {
        if(x > 0)
            ImGui::SameLine();

        // Adding invisible button to let ImGui deal with button coordinates
        ImGui::PushID(100 + x);
        if(ImGui::InvisibleButton("", colorBoxSize)) {
        }
        ImGui::PopID();

        if(!ImGui::IsItemVisible()) // Skip rendering as ImDrawList elements are
                                    // not clipped.
            continue;

        const ImVec2 p0 = ImGui::GetItemRectMin();
        const ImVec2 p1 = ImGui::GetItemRectMax();
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        draw_list->PushClipRect(p0, p1, true);

        const Rgb rgb = config.particleColors[x];
        draw_list->AddRectFilled(
            p0, p1, IM_COL32(rgb.r * 255, rgb.g * 255, rgb.b * 255, 255));
        draw_list->PopClipRect();
    }

    static int lastSelectedX = 0;
    static int lastSelectedY = 0;
    for(int y = 0; y < config.colorsCount; ++y) {
        for(int x = 0; x < config.colorsCount; ++x) {
            ImGui::PushID(y * config.colorsCount + x);
            if(ImGui::InvisibleButton("##canvas", colorBoxSize)) {
                lastSelectedX = x;
                lastSelectedY = y;
            }
            ImGui::PopID();
            if(!ImGui::IsItemVisible()) // Skip rendering as ImDrawList elements are
                                        // not clipped.
                continue;

            const ImVec2 p0 = ImGui::GetItemRectMin();
            const ImVec2 p1 = ImGui::GetItemRectMax();
            ImDrawList *draw_list = ImGui::GetWindowDrawList();
            draw_list->PushClipRect(p0, p1, true);

            const float f = config.matrix[y][x];
            const Rgb rgb = lerp(Rgb{1, 0, 0}, Rgb{0, 1, 0}, f);
            draw_list->AddRectFilled(p0, p1,
                                     IM_COL32((rgb.r + 1) / 2 * 255,
                                              (rgb.g + 1) / 2 * 255,
                                              (rgb.b + 1) / 2 * 255, 255));
            draw_list->PopClipRect();

            ImGui::SameLine();
        }

        // Adding invisible button to let ImGui deal with button coordinates
        ImGui::PushID(200 + y);
        if(ImGui::InvisibleButton("##canvas", colorBoxSize)) {
        }
        ImGui::PopID();

        if(!ImGui::IsItemVisible()) // Skip rendering as ImDrawList elements are
                                    // not clipped.
            continue;

        const ImVec2 p0 = ImGui::GetItemRectMin();
        const ImVec2 p1 = ImGui::GetItemRectMax();
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        draw_list->PushClipRect(p0, p1, true);

        const Rgb rgb = config.particleColors[y];
        draw_list->AddRectFilled(
            p0, p1, IM_COL32(rgb.r * 255, rgb.g * 255, rgb.b * 255, 255));
        draw_list->PopClipRect();
    }

    ImGui::SliderFloat("matrix", &config.matrix[lastSelectedY][lastSelectedX],
                       -1.0f, 1.0f);

    ImGui::Separator();
    ImGui::SliderFloat("rMax", &config.rMax, 0.01f, 1.0f);
    ImGui::SliderInt("size", &config.particleSize, 2, 20);

    ImGui::Separator();
    ImGui::SliderFloat("dt", &config.dt, 0.01f, 1.0f);
    ImGui::SliderFloat("Friction", &config.friction, 0.01f, 1.0f);
    ImGui::SliderFloat("k", &config.k, 0.01f, 1.0f);
}

void LayoutTestApp::RenderDebugInfo() {
    float topLeft[4] = { mGameTopLeft.x, mGameTopLeft.y, 0, 0 };
    ImGui::InputFloat2("top left", topLeft);

    float size[4] = {mGameSize.x, mGameSize.y, 0, 0};
    ImGui::InputFloat2("size", size);

    float gamePos[4] = {mGamePosition.x, mGamePosition.y, 0, 0};
    ImGui::InputFloat2("gamePos", gamePos);
}

void LayoutTestApp::RenderParticles() {
  for (size_t i = 0; i < mState.pos.size(); ++i) {
    const Position &pos = mState.pos[i];
    const int col = mState.colors[i];
    const Rgb rgb = mConfig.particleColors[col];

    const float screenX = pos.x;
    const float screenY = pos.y;

    RenderTexture(mRenderer.get(), mSpriteTexture.get(), screenX, screenY, mConfig.particleSize, rgb.r, rgb.g, rgb.b);
  }    
}
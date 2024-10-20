#pragma once

#include "IApp.h"
#include <memory>

struct SDL_Renderer;
struct SDL_Surface;
struct SDL_Texture;
struct SDL_Window;

using SDL_Texture_Handle = std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)>;
using SDL_Surface_Handle = std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)>;
using SDL_Renderer_Handle = std::unique_ptr<SDL_Renderer, void (*)(SDL_Renderer*)>;
using SDL_Window_Handle = std::unique_ptr<SDL_Window, void (*)(SDL_Window*)>;

std::unique_ptr<IApp> CreateLayoutTestApp(int16_t width, int16_t height);

class LayoutTestApp : public IApp {
public:
    LayoutTestApp(int16_t width, int16_t height, SDL_Window_Handle window, SDL_Renderer_Handle renderer, SDL_Surface_Handle surface,
		 SDL_Texture_Handle spriteTexture, SDL_Texture_Handle backBuffer);
    ~LayoutTestApp();

	void Run() override;

private:
	bool Update();
	void Render();

    int16_t mWidth;
    int16_t mHeight;
    SDL_Window_Handle mWindow;
    SDL_Renderer_Handle mRenderer;
    SDL_Surface_Handle mSurface;
    SDL_Texture_Handle mSpriteTexture;
    SDL_Texture_Handle mBackBuffer;
};
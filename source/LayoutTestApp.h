#pragma once

#include "IApp.h"

struct SDL_Window;
struct SDL_Renderer;

std::unique_ptr<IApp> CreateLayoutTestApp(int16_t width, int16_t height);

class LayoutTestApp : public IApp {
public:
    LayoutTestApp(int16_t width, int16_t height, SDL_Window *window, SDL_Renderer *renderer);
    ~LayoutTestApp();

	void Run() override;

private:
	bool Update();
	void Render();

    int16_t mWidth;
    int16_t mHeight;
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;
};
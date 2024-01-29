#pragma once

#include "IApp.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Surface;
struct SDL_Texture;

class App : public IApp
{
public:
	App(State& state, int16_t width, int16_t height, SDL_Window* window,
		SDL_Renderer* renderer,
		SDL_Surface* surface,
		SDL_Texture* spriteTexture);

	~App() override;

	bool Update() override;
	void Render() override;

	void DrawParticle(float x, float y, int size, int color) override;

private:
	void AddParticle(const float x, const float y, const int c) const;
	void ClearParticles() const;
	void UpdateParticles();
	float Force(float r, float a, float beta = 0.3f);

	static void RenderState(State& state, int& currentColor);

	State& mState;
	int mCurrentColor = 0;

	int16_t mWidth = 0;
	int16_t mHeight = 0;
	SDL_Window* mWindow = nullptr;
	SDL_Renderer* mRenderer = nullptr;
	SDL_Surface* mSurface = nullptr;
	SDL_Texture* mSpriteTexture = nullptr;
};
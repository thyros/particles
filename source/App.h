#pragma once

#include "IApp.h"


struct SDL_Window;
struct SDL_Renderer;
struct SDL_Surface;
struct SDL_Texture;

class App : public IApp
{
public:
	App(Config& config, State& state, int16_t width, int16_t height, SDL_Window* window,
		SDL_Renderer* renderer,
		SDL_Surface* surface,
		SDL_Texture* spriteTexture);

	~App() override;

	void Run() override;

private:
	bool Update();
	void Render();

	void RenderParticles();
	void DrawParticle(float x, float y, int size, int color);

	void AddParticle(const float x, const float y, const int c) const;
	void ClearParticles() const;
	void UpdateParticlesBruteForce();
	void UpdateParticlesQuadTree();

	void GenerateNewConfig();

	static void RenderConfig(Config&, int& currentColor);

	Config& mConfig;
	State& mState;

	// entt::registry mRegistry;

	int mFramesCount = 0;
	int mLastMeasurement = 0;
	int mCurrentColor = 0;

	int16_t mWidth = 0;
	int16_t mHeight = 0;
	SDL_Window* mWindow = nullptr;
	SDL_Renderer* mRenderer = nullptr;
	SDL_Surface* mSurface = nullptr;
	SDL_Texture* mSpriteTexture = nullptr;
};
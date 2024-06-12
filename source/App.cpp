#include "App.h"
#include "Math.h"
#include "Vec.h"

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>
#include <imgui.h>
#include <string>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_image.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

std::unique_ptr<IApp> CreateApp(State& state, int16_t width, int16_t height)
{
	// ####################################
	// ## SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return nullptr;
	}

	constexpr int windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
	SDL_Window* window = SDL_CreateWindow("SDL Tutorial", width, height, windowFlags);
	if (window == nullptr)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return nullptr;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return nullptr;
	}

	SDL_Surface* surface = IMG_Load("res/circle.png");
	if (surface == nullptr) {
		printf("Could not load image");
		return nullptr;
	}
	SDL_Texture* spriteTexture = SDL_CreateTextureFromSurface(renderer, surface);

	Uint32 format;
	SDL_QueryTexture(spriteTexture, &format, nullptr, nullptr, nullptr);

	// ####################################
	// ## IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsDark();

	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);


	return std::make_unique<App>(state, width, height, window, renderer, surface, spriteTexture);
}

App::App(State& state, int16_t width, int16_t height, SDL_Window* window, SDL_Renderer* renderer, SDL_Surface* surface,
	SDL_Texture* spriteTexture) : mState(state), mWidth(width), mHeight(height), mWindow(window), mRenderer(renderer), mSurface(surface), mSpriteTexture(spriteTexture)
{
	constexpr int initParticleCount = 50;
	for (int i = 0; i < initParticleCount; ++i) {
		mState.colors.push_back(rand() % mState.colorsCount);
		mState.posX.push_back(frand());
		mState.posY.push_back(frand());
		mState.velX.push_back(0);
		mState.velY.push_back(0);
	}
}

App::~App()
{
	// ####################################
	// ## IMGUI
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	// ####################################
	// ## SDL
	SDL_DestroyTexture(mSpriteTexture);
	mSpriteTexture = nullptr;

	//Deallocate surface
	SDL_DestroySurface(mSurface);
	mSurface = nullptr;

	SDL_DestroyRenderer(mRenderer);
	mRenderer = nullptr;

	//Destroy window
	SDL_DestroyWindow(mWindow);
	mWindow = nullptr;

	//Quit SDL subsystems
	SDL_Quit();
}

bool App::Update()
{
	// TODO: RSTA Remove rmb
	static bool rmb = false;

	const ImGuiIO& io = ImGui::GetIO();

	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		ImGui_ImplSDL3_ProcessEvent(&e);
		if (io.WantCaptureMouse)
		{
			continue;
		}
		if (e.type == SDL_EVENT_QUIT) {
			return true;
		}
		if (e.type == SDL_EVENT_KEY_DOWN) {
			switch (e.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				return true;

			case SDLK_1:
			case SDLK_2:
			case SDLK_3:
			case SDLK_4:
			case SDLK_5:
			case SDLK_6:
				mCurrentColor = e.key.keysym.sym - SDLK_1;
				break;

			case SDLK_SPACE:
				ClearParticles();
				break;
			}
		}
		else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
		{
			if (e.button.button == 1) {
				const int mx = e.motion.x;
				const int my = e.motion.y;

				AddParticle(static_cast<float>(mx) / mWidth, static_cast<float>(my) / mHeight, mCurrentColor);
			}
			else if (e.button.button == 3)
			{
				rmb = true;
			}
		}
		else if (e.type == SDL_EVENT_MOUSE_BUTTON_UP)
		{
			if (e.button.button == 3)
			{
				rmb = false;
			}
		}
		else if (e.type == SDL_EVENT_MOUSE_MOTION)
		{
			if (rmb) {
				const float mx = e.motion.x;
				const float my = e.motion.y;

				AddParticle(mx / mWidth, my / mHeight, mCurrentColor);
			}
		}
	}


	return false;
}

void App::Render()
{
	constexpr ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
	SDL_SetRenderDrawColor(mRenderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
	SDL_RenderClear(mRenderer);

	// const ImGuiIO& io = ImGui::GetIO();
	// SDL_SetRenderScale(mRenderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);

	UpdateParticles();

	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	RenderState(mState, mCurrentColor);
	ImGui::Render();

	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(mRenderer);


}

void App::DrawParticle(float x, float y, int size, int color)
{
	const float offset = size / 2;
	SDL_FRect dst{ .x = x - offset, .y = y - offset, .w = static_cast<float>(size), .h = static_cast<float>(size) };

	const Rgb rgb = mState.particleColors[color];
	SDL_SetTextureColorMod(mSpriteTexture, static_cast<char>(255 * rgb.r), static_cast<char>(255 * rgb.g), static_cast<char>(255 * rgb.b));
	SDL_RenderTexture(mRenderer, mSpriteTexture, nullptr, &dst);
}

void App::AddParticle(const float x, const float y, const int c) const
{
	mState.colors.push_back(c);
	mState.posX.push_back(x);
	mState.posY.push_back(y);
	mState.velX.push_back(0);
	mState.velY.push_back(0);
}

void App::ClearParticles() const
{
	mState.colors.clear();
	mState.posX.clear();
	mState.posY.clear();
	mState.velX.clear();
	mState.velY.clear();
}

void App::UpdateParticles()
{
	// update velocities
	const size_t count = mState.colors.size();
	for (size_t i = 0; i < count; ++i) {
		float totalForceX = 0;
		float totalForceY = 0;

		for (size_t j = 0; j < count; ++j) {
			if (i == j) continue;

			constexpr float wrapThreshold = 0.9f;
			float rx = mState.posX[j] - mState.posX[i];
			if (rx > wrapThreshold)
			{
				rx -= 1.0f;
			}
			else if (rx < -wrapThreshold)
			{
				rx += 1.0f;
			}

			float ry = mState.posY[j] - mState.posY[i];
			if (ry > wrapThreshold)
			{
				ry -= 1.0f;
			}
			else if (ry < -wrapThreshold)
			{
				ry += 1.0f;
			}

			const float r = Vec{ .x = rx, .y = ry }.magnitude();
			if (r > 0 && r < mState.rMax) {
				const float f = Force(r / mState.rMax, mState.matrix[mState.colors[i]][mState.colors[j]]);
				totalForceX += rx / r * f;
				totalForceY += ry / r * f;
			}
		}

		totalForceX *= mState.rMax * mState.forceFactor;
		totalForceY *= mState.rMax * mState.forceFactor;

		mState.velX[i] *= mState.frictionFactor;
		mState.velY[i] *= mState.frictionFactor;

		mState.velX[i] += totalForceX * mState.dt;
		mState.velY[i] += totalForceY * mState.dt;
	}


	// update positions
	for (size_t i = 0; i < count; ++i) {
		mState.posX[i] += mState.velX[i] * mState.dt;
		mState.posY[i] += mState.velY[i] * mState.dt;
		mState.posX[i] = warp(mState.posX[i]);
		mState.posY[i] = warp(mState.posY[i]);
	}

	// render
	for (size_t i = 0; i < count; ++i) {
		const float screenX = mState.posX[i] * mWidth;
		const float screenY = mState.posY[i] * mHeight;

		DrawParticle(screenX, screenY, mState.particleSize, mState.colors[i]);
	}
}

float App::Force(float r, float a, float beta) {
	if (r < beta) {
		return r / beta - 1;
	}
	if (beta < r && r < 1) {
		return a * (1 - std::abs(2 * r - 1 - beta) / (1 - beta));
	}
	return 0;
}

void App::RenderState(State& state, int& currentColor)
{
	ImGui::Begin("Particles!");									 // Create a window called "Hello, world!" and append into it.
	ImGui::Text("Particles: %zu", state.colors.size());
	ImGui::Text("Current Color: %i", currentColor + 1);               // Display some text (you can use a format strings too)

	for (size_t i = 0; i < state.colorsCount; ++i)
	{
		ImGuiColorEditFlags misc_flags = 0;
		float color[3]{ state.particleColors[i].r, state.particleColors[i].g, state.particleColors[i].b };
		std::string id = "Color " + std::to_string(i + 1);
		ImGui::ColorEdit3(id.c_str(), (float*)&color, misc_flags);
		state.particleColors[i].r = color[0];
		state.particleColors[i].g = color[1];
		state.particleColors[i].b = color[2];
	}

	static int lastSelectedX = 0;
	static int lastSelectedY = 0;
	for (int y = 0; y < state.colorsCount; ++y)
	{
		constexpr ImVec2 size(25.0f, 25.0f);
		for (int x = 0; x < state.colorsCount; ++x)
		{
			if (x > 0) ImGui::SameLine();
			ImGui::PushID(y * 10 + x);
			if (ImGui::InvisibleButton("##canvas", size)) {
				lastSelectedX = x;
				lastSelectedY = y;
			}
			ImGui::PopID();
			if (!ImGui::IsItemVisible()) // Skip rendering as ImDrawList elements are not clipped.
				continue;

			const ImVec2 p0 = ImGui::GetItemRectMin();
			const ImVec2 p1 = ImGui::GetItemRectMax();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			draw_list->PushClipRect(p0, p1, true);

			const float f = state.matrix[y][x];
			const Rgb rgb = lerp(Rgb{ 1, 0, 0 }, Rgb{ 0, 1, 0 }, f);
			draw_list->AddRectFilled(p0, p1, IM_COL32((rgb.r + 1) / 2 * 255, (rgb.g + 1) / 2 * 255, (rgb.b + 1) / 2 * 255, 255));
			draw_list->PopClipRect();
		}
	}

	ImGui::SliderFloat("matrix", &state.matrix[lastSelectedY][lastSelectedX], -1.0f, 1.0f);
	ImGui::SliderFloat("rMax", &state.rMax, 0.01f, 1.0f);
	ImGui::SliderInt("size", &state.particleSize, 2, 20);



	ImGui::End();
}

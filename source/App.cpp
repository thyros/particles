#include "App.h"
#include "ConfigFunctions.h"
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
#include <format>

std::unique_ptr<IApp> CreateApp(Config &config, State &state, int16_t width, int16_t height)
{
	// ####################################
	// ## SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
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

	SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return nullptr;
	}

	SDL_Surface *surface = IMG_Load("res/circle.png");
	if (surface == nullptr)
	{
		printf("Could not load image");
		return nullptr;
	}
	SDL_Texture *spriteTexture = SDL_CreateTextureFromSurface(renderer, surface);

	Uint32 format;
	SDL_QueryTexture(spriteTexture, &format, nullptr, nullptr, nullptr);

	// ####################################
	// ## IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
	ImGui::StyleColorsDark();

	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);

	return std::make_unique<App>(config, state, width, height, window, renderer, surface, spriteTexture);
}

App::App(Config &config, State &state, int16_t width, int16_t height, SDL_Window *window, SDL_Renderer *renderer, SDL_Surface *surface,
		 SDL_Texture *spriteTexture) : mConfig(config), mState(state), mWidth(width), mHeight(height), mWindow(window), mRenderer(renderer), mSurface(surface), mSpriteTexture(spriteTexture)
{
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

void App::Run()
{
	constexpr int Second = 1000;

	bool quit = false;
	mLastMeasurement = SDL_GetTicks();

	while (!quit)
	{
		++mFramesCount;

		const auto now = SDL_GetTicks();
		if (now - mLastMeasurement > Second)
		{
			const std::string title = std::format("Particles: {} FPS: {}", mState.colors.size(), mFramesCount);
			SDL_SetWindowTitle(mWindow, title.c_str());

			mLastMeasurement = now;
			mFramesCount = 0;
		}

		quit = Update();
		Render();
	}
}

bool App::Update()
{
	// TODO: RSTA Remove rmb
	static bool rmb = false;
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
		if (e.type == SDL_EVENT_KEY_DOWN)
		{
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

			case SDLK_z:
				GenerateNewConfig();
				break;
			}			
		}
		else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
		{
			if (e.button.button == 1)
			{
				const int mx = e.motion.x;
				const int my = e.motion.y;

				AddParticle(static_cast<float>(mx), static_cast<float>(my), mCurrentColor);
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
			if (rmb)
			{
				const float mx = e.motion.x;
				const float my = e.motion.y;

				AddParticle(mx, my, mCurrentColor);
			}
		}
	}

	UpdateParticles();
	// UpdateParticlesOld();

	return false;
}

void App::Render()
{
	constexpr ImVec4 clearColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	SDL_SetRenderDrawColor(mRenderer, (Uint8)(clearColor.x * 255), (Uint8)(clearColor.y * 255), (Uint8)(clearColor.z * 255), (Uint8)(clearColor.w * 255));
	SDL_RenderClear(mRenderer);

	RenderParticles();

	// const ImGuiIO& io = ImGui::GetIO();
	// SDL_SetRenderScale(mRenderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);

	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	static bool open = false;
	ImGui::Begin("Particles!", &open); // Create a window called "Hello, world!" and append into it.
	RenderConfig(mConfig, mCurrentColor);
	ImGui::End();

	ImGui::Render();

	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(mRenderer);
}

void App::RenderParticles() {
	for (size_t i = 0; i < mState.colors.size(); ++i) {
		const Vec& pos = mState.pos[i];
		const int col = mState.colors[i];

		const float screenX = pos.x;
		const float screenY = pos.y;

		DrawParticle(screenX, screenY, mConfig.particleSize, col);
	}
}

void App::DrawParticle(float x, float y, int size, int color)
{
	const float offset = size / 2;
	SDL_FRect dst{.x = x - offset, .y = y - offset, .w = static_cast<float>(size), .h = static_cast<float>(size)};

	const Rgb rgb = mConfig.particleColors[color];
	SDL_SetTextureColorMod(mSpriteTexture, static_cast<char>(255 * rgb.r), static_cast<char>(255 * rgb.g), static_cast<char>(255 * rgb.b));
	SDL_RenderTexture(mRenderer, mSpriteTexture, nullptr, &dst);
}

void App::AddParticle(const float x, const float y, const int c) const
{
	mState.colors.push_back(c);
	mState.pos.push_back(Vec {.x = x, .y = y} );
	mState.vel.push_back(Vec {} );
}

void App::ClearParticles() const
{
	mState.colors.clear();
	mState.pos.clear();
	mState.vel.clear();
}

void App::GenerateNewConfig() {
	mConfig.minDistances = generateDistances(mConfig.colorsCount);
	mConfig.forces = generateForces(mConfig.colorsCount);
	mConfig.radii = generateRadii(mConfig.colorsCount);
}

void App::UpdateParticles() {
	const size_t count = mState.colors.size();
	for (size_t i = 0; i < count; ++i) {
		Vec totalForce;

		for (size_t j = 0; j < count; ++j) {
			if (i == j) {
				continue;
			}

			Vec direction = mState.pos[j] - mState.pos[i];

			if (direction.x > 0.5f * mWidth) {
				direction.x -= mWidth;
			} else if (direction.x < -0.5f * mWidth) {
				direction.x += mWidth;
			}
			if (direction.y > 0.5f * mHeight) {
				direction.y -= mHeight;
			} else if (direction.y < -0.5f * mHeight) {
				direction.y += mHeight;
			}

			const float distance = direction.magnitude();
			direction.normalize();

			const int c1 = mState.colors[i];
			const int c2 = mState.colors[j];
			if (distance < mConfig.minDistances[c1][c2]) {
				float factor = std::abs(mConfig.forces[c1][c2]) * -3;
				factor *= map(distance, 0, mConfig.minDistances[c1][c2], 1, 0);
				factor *= mConfig.k;

				Vec force = distance != 0 ? direction : randomVec();
				force.mul(factor);
				totalForce.add(force);
			}

			if (distance < mConfig.radii[c1][c2]) {
				float factor = mConfig.forces[c1][c2];
				factor *= map(distance, 0, mConfig.radii[c1][c2], 1, 0);
				factor *= mConfig.k;

				Vec force = distance != 0 ? direction : randomVec();
				force.mul(factor);
				totalForce.add(force);
			}
		}

		mState.vel[i].mul(mConfig.friction);
		mState.vel[i].add(totalForce.mul(mConfig.dt));

		mState.pos[i].add(mState.vel[i]);
		mState.pos[i] = wrap(mState.pos[i], Vec {static_cast<float>(mWidth), static_cast<float>(mHeight)});
	}	

}

void App::UpdateParticlesOld()
{
	const auto Force = [](float r, float a, float beta)
	{
		if (r < beta)
		{
			return r / beta - 1;
		}
		if (beta < r && r < 1)
		{
			return a * (1 - std::abs(2 * r - 1 - beta) / (1 - beta));
		}
		return 0.0f;
	};

	// update velocities
	const size_t count = mState.colors.size();
	for (size_t i = 0; i < count; ++i)
	{
		float totalForceX = 0;
		float totalForceY = 0;

		for (size_t j = 0; j < count; ++j)
		{
			if (i == j)
				continue;

			constexpr float wrapThreshold = 0.9f;
			float rx = mState.pos[j].x - mState.pos[i].x;
			if (rx > wrapThreshold)
			{
				rx -= 1.0f;
			}
			else if (rx < -wrapThreshold)
			{
				rx += 1.0f;
			}

			float ry = mState.pos[j].y - mState.pos[i].y;
			if (ry > wrapThreshold)
			{
				ry -= 1.0f;
			}
			else if (ry < -wrapThreshold)
			{
				ry += 1.0f;
			}

			const float r = Vec{.x = rx, .y = ry}.magnitude();
			if (r > 0 && r < mConfig.rMax)
			{
				const float f = Force(r / mConfig.rMax, mConfig.matrix[mState.colors[i]][mState.colors[j]], 0.3f);
				totalForceX += rx / r * f;
				totalForceY += ry / r * f;
			}
		}

		totalForceX *= mConfig.rMax * mConfig.forceFactor;
		totalForceY *= mConfig.rMax * mConfig.forceFactor;

		mState.vel[i].mul(mConfig.frictionFactor);

		mState.vel[i].add(Vec{.x = totalForceX * mConfig.dt, .y = totalForceY * mConfig.dt});
	}

	// update positions
	for (size_t i = 0; i < count; ++i)
	{
		mState.pos[i].add(Vec{.x = mState.vel[i].x * mConfig.dt, .y = mState.vel[i].y * mConfig.dt});
		mState.pos[i].x = warp(mState.pos[i].x);
		mState.pos[i].y = warp(mState.pos[i].y);
	}

	// render
	for (size_t i = 0; i < count; ++i)
	{
		const float screenX = mState.pos[i].x;
		const float screenY = mState.pos[i].y;

		DrawParticle(screenX, screenY, mConfig.particleSize, mState.colors[i]);
	}
}

void App::RenderConfig(Config &config, int &currentColor)
{
	constexpr ImVec2 colorBoxSize(25.0f, 25.0f);

	ImGui::Text("Current Color: %i", currentColor + 1); // Display some text (you can use a format strings too)

	// Particle colors
	for (size_t i = 0; i < config.colorsCount; ++i)
	{
		ImGuiColorEditFlags misc_flags = 0;
		float color[3]{config.particleColors[i].r, config.particleColors[i].g, config.particleColors[i].b};
		std::string id = "Color " + std::to_string(i + 1);
		ImGui::ColorEdit3(id.c_str(), (float *)&color, misc_flags);
		config.particleColors[i].r = color[0];
		config.particleColors[i].g = color[1];
		config.particleColors[i].b = color[2];
	}

	// Matrix
	for (int x = 0; x < config.colorsCount; ++x)
	{
		if (x > 0)
			ImGui::SameLine();

		// Adding invisible button to let ImGui deal with button coordinates
		if (ImGui::InvisibleButton("##canvas", colorBoxSize))
		{
		}

		if (!ImGui::IsItemVisible()) // Skip rendering as ImDrawList elements are not clipped.
			continue;

		const ImVec2 p0 = ImGui::GetItemRectMin();
		const ImVec2 p1 = ImGui::GetItemRectMax();
		ImDrawList *draw_list = ImGui::GetWindowDrawList();
		draw_list->PushClipRect(p0, p1, true);

		const Rgb rgb = config.particleColors[x];
		draw_list->AddRectFilled(p0, p1, IM_COL32(rgb.r * 255, rgb.g * 255, rgb.b * 255, 255));
		draw_list->PopClipRect();
	}

	static int lastSelectedX = 0;
	static int lastSelectedY = 0;
	for (int y = 0; y < config.colorsCount; ++y)
	{
		for (int x = 0; x < config.colorsCount; ++x)
		{
			ImGui::PushID(y * 10 + x);
			if (ImGui::InvisibleButton("##canvas", colorBoxSize))
			{
				lastSelectedX = x;
				lastSelectedY = y;
			}
			ImGui::PopID();
			if (!ImGui::IsItemVisible()) // Skip rendering as ImDrawList elements are not clipped.
				continue;

			const ImVec2 p0 = ImGui::GetItemRectMin();
			const ImVec2 p1 = ImGui::GetItemRectMax();
			ImDrawList *draw_list = ImGui::GetWindowDrawList();
			draw_list->PushClipRect(p0, p1, true);

			const float f = config.matrix[y][x];
			const Rgb rgb = lerp(Rgb{1, 0, 0}, Rgb{0, 1, 0}, f);
			draw_list->AddRectFilled(p0, p1, IM_COL32((rgb.r + 1) / 2 * 255, (rgb.g + 1) / 2 * 255, (rgb.b + 1) / 2 * 255, 255));
			draw_list->PopClipRect();

			ImGui::SameLine();
		}

		// Adding invisible button to let ImGui deal with button coordinates
		if (ImGui::InvisibleButton("##canvas", colorBoxSize))
		{
		}

		if (!ImGui::IsItemVisible()) // Skip rendering as ImDrawList elements are not clipped.
			continue;

		const ImVec2 p0 = ImGui::GetItemRectMin();
		const ImVec2 p1 = ImGui::GetItemRectMax();
		ImDrawList *draw_list = ImGui::GetWindowDrawList();
		draw_list->PushClipRect(p0, p1, true);

		const Rgb rgb = config.particleColors[y];
		draw_list->AddRectFilled(p0, p1, IM_COL32(rgb.r * 255, rgb.g * 255, rgb.b * 255, 255));
		draw_list->PopClipRect();
	}

	ImGui::SliderFloat("matrix", &config.matrix[lastSelectedY][lastSelectedX], -1.0f, 1.0f);

	ImGui::Separator();
	ImGui::SliderFloat("rMax", &config.rMax, 0.01f, 1.0f);
	ImGui::SliderInt("size", &config.particleSize, 2, 20);

	ImGui::Separator();
	ImGui::SliderFloat("dt", &config.dt, 0.01f, 1.0f);
	ImGui::SliderFloat("Friction", &config.friction, 0.01f, 1.0f);
	ImGui::SliderFloat("k", &config.k, 0.01f, 1.0f);
}

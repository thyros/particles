/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_render.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

//Screen dimension constants
constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 960;
constexpr int COUNT = 100;
constexpr float MAX_SPEED = 1;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = nullptr;

SDL_Renderer* gRenderer = nullptr;

//The image we will load and show on the screen
SDL_Surface* gXOut = nullptr;
SDL_Texture* gSpriteTexture = nullptr;

///////////////////////////////////////////////////////////////

struct Rgb {
	float r;
	float g;
	float b;
};
using Rgbs = std::vector<Rgb>;

Rgb lerp(const Rgb& lhs, const Rgb& rhs, float t)
{
	t = (t + 1) / 2;
	const float r = (1.0f - t) * lhs.r + t * rhs.r;
	const float g = (1.0f - t) * lhs.g + t * rhs.g;
	const float b = (1.0f - t) * lhs.b + t * rhs.b;
	return Rgb{ r, g, b };
}

float frand() {
	constexpr int max = 10000;
	return static_cast<float>(rand() % max) / max;
}

using Matrix = std::vector<std::vector<float>>;
Matrix makeRandomMatrix(const int m) {
	Matrix matrix = std::vector(m, std::vector(m, 0.0f));

	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < m; ++j) {
			const float r = frand() * 2 - 1;
			matrix[i][j] = r;
		}
	}

	return matrix;
}

Matrix makeIdentityMatrix(const int m)
{
	Matrix matrix = std::vector(m, std::vector(m, 0.0f));

	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < m; ++j) {
			if (i == j) 
				matrix[i][j] = 1;
		}
	}

	return matrix;
}

Rgbs generateRandomColors(const int c) {
	Rgbs rgbs;
	for (int i = 0; i < c; ++i) {
		rgbs.emplace_back(frand(), frand(), frand());
	}
	return rgbs;
}

const int initParticleCount = 0;
const float dt = 0.001f;
const float frictionHalfLife = 0.04f;
float rMax = 0.1f;
const int colorsCount = 5;
Matrix matrix = makeIdentityMatrix(colorsCount);
Rgbs rgbs = generateRandomColors(colorsCount);
const float frictionFactor = std::pow(0.5f, dt / frictionHalfLife);
const float forceFactor = 10;
int partcleSize = 4.0f;

std::vector<int> colors;
std::vector<float> posX;
std::vector<float> posY;
std::vector<float> velX;
std::vector<float> velY;

void addParticle(const float x, const float y, const int c)
{
	colors.push_back(c);
	posX.push_back(x);
	posY.push_back(y);
	velX.push_back(0);
	velY.push_back(0);
}

void clearParticles()
{
	colors.clear();
	posX.clear();
	posY.clear();
	velX.clear();
	velY.clear();
}

///////////////////////////////////////////////////////////////

struct Vec {
	float x;
	float y;

	void add(const Vec& v) {
		x += v.x;
		y += v.y;
	}

	void div(float f) {
		x /= f;
		y /= f;
	}

	float magnitude() {
		return std::sqrt(x * x + y * y);
	}

	void normalize() {
		float m = magnitude();

		if (m > 0) {
			div(m);
		}
	}

	float distance(const Vec& v) {
		const float dx = x - v.x;
		const float dy = y - v.y;
		const float dist = std::sqrt(dx * dx + dy * dy);
		return dist;
	}
};

Vec sub(const Vec& v1, const Vec& v2) {
	return Vec{ v1.x - v2.x, v1.y - v2.y };
}

struct Particle {
	Vec pos;
	float vx = 0, vy = 0;
	int size = 32;
};
using Particles = std::vector<Particle>;

Particles createParticles(int count) {
	Particles particles(count, Particle{});

	for (Particle& p : particles) {
		p.pos.x = rand() % SCREEN_WIDTH;
		p.pos.y = rand() % SCREEN_HEIGHT;

		p.vx = static_cast<float>(rand() % 1000 - 500) / 1000;
		p.vy = static_cast<float>(rand() % 1000 - 500) / 1000;
	}

	return particles;
}

void drawParticle(float x, float y, int size, int color) {
	const float offset = size / 2;
	SDL_Rect dst{ .x = static_cast<int>(x - offset), .y = static_cast<int>(y - offset), .w = size, .h = size };

	const Rgb rgb = rgbs[color];
	SDL_SetTextureColorMod(gSpriteTexture, static_cast<char>(255 * rgb.r), static_cast<char>(255 * rgb.g), static_cast<char>(255 * rgb.b));
	SDL_RenderCopy(gRenderer, gSpriteTexture, nullptr, &dst);
}

void updatePosition(Particle& p, float dt) {
	p.pos.x += p.vx * dt;
	if (p.pos.x < 0) p.pos.x += SCREEN_WIDTH;
	else if (p.pos.x >= SCREEN_WIDTH) p.pos.x -= SCREEN_WIDTH;
	p.pos.y += p.vy * dt;
	if (p.pos.y < 0) p.pos.y += SCREEN_HEIGHT;
	else if (p.pos.y >= SCREEN_HEIGHT) p.pos.y -= SCREEN_HEIGHT;
}

void steer(Particles& particles) {
	for (size_t i = 0; i < particles.size(); ++i) {
		float vx = 0;
		float vy = 0;

		for (size_t j = 0; j < particles.size(); ++j) {
			if (i == j) {
				continue;
			}

			vx += particles[j].vx;
			vy += particles[j].vy;
		}

		const float nx = vx / particles.size();
		const float ny = vy / particles.size();

		const float m = std::sqrt(nx * nx + ny * ny);
		particles[i].vx = nx / m * MAX_SPEED;
		particles[i].vy = ny / m * MAX_SPEED;
	}
}

void separate(Particles& particles, float distance) {
	for (size_t i = 0; i < particles.size(); ++i) {

		Vec steer{ 0, 0 };

		for (size_t j = 0; j < particles.size(); ++j) {
			if (i == j) {
				continue;
			}

			const float d = particles[i].pos.distance(particles[j].pos);

			Vec diff = sub(particles[i].pos, particles[j].pos);
			diff.normalize();
			diff.div(d);
		}
	}
}

bool init()
{
	for (int i = 0; i < initParticleCount; ++i) {
		colors.push_back(rand() % colorsCount);
		posX.push_back(frand());
		posY.push_back(frand());
		velX.push_back(0);
		velY.push_back(0);
	}

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		return false;
	}

    //Create window
	const int window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN;
    gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, window_flags );
    if( gWindow == nullptr )
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }

    gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
    if( gRenderer == NULL )
    {
        printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
        return false;
    }

	gXOut = IMG_Load("res/circle.png");
	if (gXOut == nullptr) {
		printf("Could not load image");
		return false;
	}
    gSpriteTexture = SDL_CreateTextureFromSurface(gRenderer, gXOut);

	return true;
}

void close()
{
    SDL_DestroyTexture(gSpriteTexture);
    gSpriteTexture = nullptr;

	//Deallocate surface
	SDL_FreeSurface( gXOut );
	gXOut = nullptr;

    SDL_DestroyRenderer( gRenderer );
    gRenderer = nullptr;

	//Destroy window
	SDL_DestroyWindow( gWindow );
	gWindow = nullptr;

	//Quit SDL subsystems
	SDL_Quit();
}

float force(float r, float a, float beta = 0.3f) {
	if (r < beta) {
		return r / beta - 1;
	}
	else if (beta < r && r < 1) {
		return a * (1 - std::abs(2 * r - 1 - beta) / (1 - beta));
	}
	return 0;	
}

float warp(float x)
{
	if (x < 0)
		return x + 1.0f;
	if (x > 1)
		return x - 1.0f;
	return x;
}

void loop() {
	// update velocities
	const size_t count = colors.size();
	for (size_t i = 0; i < count; ++i) {
		float totalForceX = 0;
		float totalForceY = 0;

		for (size_t j = 0; j < count; ++j) {
			if (i == j) continue;

			constexpr float wrapThreshold = 0.9f;
			float rx = posX[j] - posX[i];
			if (rx > wrapThreshold)
			{
				rx -= 1.0f;
			} else if (rx < -wrapThreshold)
			{
				rx += 1.0f;
			}

			float ry = posY[j] - posY[i];
			if (ry > wrapThreshold)
			{
				ry -= 1.0f;
			} else if (ry < -wrapThreshold)
			{
				ry += 1.0f;
			}

			const float r = Vec{ .x = rx, .y = ry }.magnitude();
			if (r > 0 && r < rMax) {
				const float f = force(r / rMax, matrix[colors[i]][colors[j]]);
				totalForceX += rx / r * f;
				totalForceY += ry / r * f;
			}
		}

		totalForceX *= rMax * forceFactor;
		totalForceY *= rMax * forceFactor;

		velX[i] *= frictionFactor;
		velY[i] *= frictionFactor;

		velX[i] += totalForceX * dt;
		velY[i] += totalForceY * dt;
	}


	// update positions
	for (size_t i = 0; i < count; ++i) {
		posX[i] += velX[i] * dt;
		posY[i] += velY[i] * dt;
		posX[i] = warp(posX[i]);
		posY[i] = warp(posY[i]);
	}

	// render
	for (size_t i = 0; i < count; ++i) {
		const float screenX = posX[i] * SCREEN_WIDTH;
		const float screenY = posY[i] * SCREEN_HEIGHT;

		drawParticle(screenX, screenY, partcleSize, colors[i]);
	}
}

int main( int argc, char* args[] )
{

	int currentColor = 0;
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		ImGui::StyleColorsDark();

		ImGui_ImplSDL2_InitForSDLRenderer(gWindow, gRenderer);
		ImGui_ImplSDLRenderer2_Init(gRenderer);

		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;

		Particles particles = createParticles(100);

		bool rmb = false;

		//While application is running
		while( !quit )
		{
			//Handle events on queue
			while( SDL_PollEvent( &e ) != 0 )
			{
				ImGui_ImplSDL2_ProcessEvent(&e);
				if (io.WantCaptureMouse)
				{
					continue;
				}
				if (e.type == SDL_KEYDOWN) {
					switch (e.key.keysym.sym)
					{
					case SDLK_ESCAPE: 
						quit = true;
						break;

					case SDLK_1:
					case SDLK_2:
					case SDLK_3:
					case SDLK_4:
					case SDLK_5:
					case SDLK_6:
						currentColor = e.key.keysym.sym - SDLK_1;
						break;

					case SDLK_SPACE:
						clearParticles();
						break;
					}
				} else if (e.type == SDL_MOUSEBUTTONDOWN)
				{
					if (e.button.button == 1) {
						const int mx = e.motion.x;
						const int my = e.motion.y;

						addParticle(static_cast<float>(mx) / SCREEN_WIDTH, static_cast<float>(my) / SCREEN_HEIGHT, currentColor);
					} else if (e.button.button == 3)
					{
						rmb = true;
					}
				} else if (e.type == SDL_MOUSEBUTTONUP)
				{
					if (e.button.button == 3)
					{
						rmb = false;
					}
				} else if (e.type == SDL_MOUSEMOTION)
				{
					if (rmb) {
						const int mx = e.motion.x;
						const int my = e.motion.y;

						addParticle(static_cast<float>(mx) / SCREEN_WIDTH, static_cast<float>(my) / SCREEN_HEIGHT, currentColor);
					}
				}
			}

			ImGui_ImplSDLRenderer2_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();

            ImGui::Begin("Particles!");									 // Create a window called "Hello, world!" and append into it.
			ImGui::Text("Particles: %zu", colors.size());
            ImGui::Text("Current Color: %i", currentColor + 1);               // Display some text (you can use a format strings too)

			for (size_t i = 0; i < colorsCount; ++i)
			{
				ImGuiColorEditFlags misc_flags = 0;
				float color[3]{ rgbs[i].r, rgbs[i].g, rgbs[i].b };
				std::string id = "Color " + std::to_string(i + 1);
				ImGui::ColorEdit3(id.c_str(), (float*)&color, misc_flags);
				rgbs[i].r = color[0];
				rgbs[i].g = color[1];
				rgbs[i].b = color[2];
			}

			static int lastSelectedX = 0;
			static int lastSelectedY = 0;
			for (int y = 0; y < colorsCount; ++y)
			{
				constexpr ImVec2 size(25.0f, 25.0f);
				for (int x = 0; x < colorsCount; ++x)
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

					const float f = matrix[y][x];
					const Rgb rgb = lerp(Rgb { 1, 0, 0 }, Rgb{ 0, 1, 0 }, f);
					draw_list->AddRectFilled(p0, p1, IM_COL32((rgb.r + 1) / 2 * 255, (rgb.g + 1) / 2 * 255, (rgb.b + 1) / 2 * 255, 255));
					draw_list->PopClipRect();
				}
			}

			ImGui::SliderFloat("matrix", &matrix[lastSelectedY][lastSelectedX], -1.0f, 1.0f);
			ImGui::SliderFloat("rMax", &rMax, 0.01f, 1.0f);
			ImGui::SliderInt("size", &partcleSize, 2, 20);



			ImGui::End();
			ImGui::Render();

			constexpr ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

			//Clear screen
			ImGui::Render();

			SDL_RenderSetScale(gRenderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
			SDL_SetRenderDrawColor(gRenderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
			SDL_RenderClear(gRenderer);

			loop();

			ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
			SDL_RenderPresent(gRenderer);
		}
	}

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();	

	//Free resources and close SDL
	close();

	return 0;
}

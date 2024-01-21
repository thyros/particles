/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_render.h>
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
	char r;
	char g;
	char b;
};
using Rgbs = std::vector<Rgb>;

float frand() {
	constexpr int max = 10000;
	return static_cast<float>(rand() % max) / max;
}

using Matrix = std::vector<std::vector<float>>;
Matrix makeRandomMatrix(const int m) {
	Matrix matrix = std::vector(m, std::vector(m, 0.0f));

	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < m; ++j) {
			matrix[i][j] = frand() * 2 - 1;
		}
	}

	return matrix;
}

Rgbs generateRandomColors(const int c) {
	Rgbs rgbs;
	for (int i = 0; i < c; ++i) {
		rgbs.emplace_back(rand() % 256, rand() % 256, rand() % 256);
	}
	return rgbs;
}

const int n = 1500;
const float dt = 0.01f;
const float frictionHalfLife = 0.04f;
const float rMax = 0.5f;
const int colorsCount = 4;
const Matrix matrix = makeRandomMatrix(colorsCount);
const Rgbs rgbs = generateRandomColors(colorsCount);
const float frictionFactor = std::pow(0.5f, dt / frictionHalfLife);
const float forceFactor = 10;

std::vector<int> colors;
std::vector<float> posX;
std::vector<float> posY;
std::vector<float> velX;
std::vector<float> velY;

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
	const int offset = size / 2;
	SDL_Rect dst{ .x = static_cast<int>(x - offset), .y = static_cast<int>(y - offset), .w = size, .h = size };

	const Rgb rgb = rgbs[color];
	SDL_SetTextureColorMod(gSpriteTexture, rgb.r, rgb.g, rgb.b);
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
	for (int i = 0; i < n; ++i) {
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
    gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
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

void loop() {

	// update velocities
	for (int i = 0; i < n; ++i) {
		float totalForceX = 0;
		float totalForceY = 0;

		for (int j = 0; j < n; ++j) {
			if (i == j) continue;

			const float rx = posX[j] - posX[i];
			const float ry = posY[j] - posY[i];

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
	for (int i = 0; i < n; ++i) {
		posX[i] += velX[i] * dt;
		posY[i] += velY[i] * dt;
	}

	// render
	for (int i = 0; i < n; ++i) {
		const float screenX = posX[i] * SCREEN_WIDTH;
		const float screenY = posY[i] * SCREEN_HEIGHT;

		drawParticle(screenX, screenY, 4, colors[i]);
	}
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;

		Particles particles = createParticles(100);

		//While application is running
		while( !quit )
		{
			//Handle events on queue
			while( SDL_PollEvent( &e ) != 0 )
			{
                //if( e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP )
                //{
                //    //Get mouse position
                //    SDL_GetMouseState( &mx, &my );
                //}

				if (e.type == SDL_KEYDOWN) {
					quit = true;
				}

				//User requests quit
				if( e.type == SDL_QUIT )
				{
					quit = true;
				}
			}

			//Clear screen
			SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
			SDL_RenderClear( gRenderer );


			loop();

			//steer(particles);

			//for (Particle& p : particles) {
			//	updatePosition(p, 1.0f);
			//}


			//for (const Particle& p : particles) {
			//	drawParticle(p);
			//}


			//Update screen
			SDL_RenderPresent( gRenderer );
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}

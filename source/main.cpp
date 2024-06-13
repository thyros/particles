/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/


#include "IApp.h"
#include "ConfigFunctions.h"
#include "StateFunctions.h"
#include <SDL3/SDL_main.h>

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 960;

///////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	constexpr int colorsCount = 5;
	constexpr int particlesCount = 1000;

	Config config;
	config.colorsCount = colorsCount;
	config.particleColors = generateRandomColors(config.colorsCount);
	config.matrix = generateRandomMatrix(config.colorsCount);

	State state = generateState(particlesCount, config.colorsCount);

	const std::unique_ptr<IApp> app = CreateApp(config, state, SCREEN_WIDTH, SCREEN_HEIGHT);
	app->Run();

	return 0;
}

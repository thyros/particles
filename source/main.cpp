/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/


#include "IApp.h"
#include "ConfigFunctions.h"
#include "StateFunctions.h"

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 960;

///////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	constexpr int colorsCount = 6;
	constexpr int particlesCount = 1000;

	Config config;
	config.colorsCount = colorsCount;
	config.particleColors = generateRandomColors(config.colorsCount);
	config.matrix = generateRandomMatrix(config.colorsCount);
	config.minDistances = generateDistances(config.colorsCount);
	config.forces = generateForces(config.colorsCount);
	config.radii = generateRadii(config.colorsCount);

	State state = generateRandomState(particlesCount, config.colorsCount, SCREEN_WIDTH, SCREEN_HEIGHT);

	const std::unique_ptr<IApp> app = CreateApp(config, state, SCREEN_WIDTH, SCREEN_HEIGHT);
	app->Run();

	return 0;
}

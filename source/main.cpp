#include "IApp.h"
#include "ConfigFunctions.h"
#include "StateFunctions.h"

#include "LayoutTestApp.h"
#include <memory>

int main(int argc, char **argv) {

	constexpr int width = 1280;
	constexpr int height = 960;

	constexpr int colorsCount = 6;
	Config config;
	config.colorsCount = colorsCount;
	config.particleColors = generateRandomColors(config.colorsCount);
	config.matrix = generateRandomMatrix(config.colorsCount);
	config.minDistances = generateDistances(config.colorsCount);
	config.forces = generateForces(config.colorsCount);
	config.radii = generateRadii(config.colorsCount);

	constexpr int particlesCount = 1;
	State state = generateRandomState(particlesCount, config.colorsCount, width, height);

	const std::unique_ptr<IApp> app = CreateLayoutTestApp(config, state, width, height);

	app->Run();
	return 0;
}
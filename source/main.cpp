/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

#include "Math.h"
#include "IApp.h"
#include "State.h"

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 960;

///////////////////////////////////////////////////////////////

Matrix generateRandomMatrix(const int m) {
	Matrix matrix = std::vector(m, std::vector(m, 0.0f));

	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < m; ++j) {
			const float r = frand() * 2 - 1;
			matrix[i][j] = r;
		}
	}

	return matrix;
}

Matrix generateIdentityMatrix(const int m)
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

ParticleColors generateRandomColors(const int c) {
	ParticleColors particleColors;
	for (int i = 0; i < c; ++i) {
		particleColors.emplace_back(frand(), frand(), frand());
	}
	return particleColors;
}

///////////////////////////////////////////////////////////////

int main()
{
	State state;
	state.colorsCount = 5;
	state.particleColors = generateRandomColors(state.colorsCount);
	state.matrix = generateRandomMatrix(state.colorsCount);

	const std::unique_ptr<IApp> app = CreateApp(state, SCREEN_WIDTH, SCREEN_HEIGHT);

	bool quit = false;

	while( !quit )
	{
		quit = app->Update();
		app->Render();
	}

	return 0;
}

#pragma once

#include <cmath>
#include <vector>

struct Rgb
{
	float r;
	float g;
	float b;
};

using ParticleColors = std::vector<Rgb>;
using Matrix = std::vector<std::vector<float>>;

struct Config {
	int colorsCount = 5;
	ParticleColors particleColors;

	float dt = 0.75f;
	float frictionHalfLife = 0.04f;
	float rMax = 0.1f;
	float frictionFactor = std::pow(0.5f, dt / frictionHalfLife);
	float forceFactor = 10;
	int particleSize = 4.0f;

	Matrix matrix;

	Matrix minDistances;
	Matrix forces;
	Matrix radii;
	float k = 0.05f;
	float friction = 0.85f;
};
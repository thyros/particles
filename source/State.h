#pragma once

#include <cmath>
#include <vector>

struct Rgb {
	float r;
	float g;
	float b;
};
using ParticleColors = std::vector<Rgb>;
using Matrix = std::vector<std::vector<float>>;

struct State
{
	int colorsCount = 5;
	ParticleColors particleColors;

	float dt = 0.001f;
	float frictionHalfLife = 0.04f;
	float rMax = 0.1f;
	float frictionFactor = std::pow(0.5f, dt / frictionHalfLife);
	float forceFactor = 10;
	int particleSize = 4.0f;

	Matrix matrix;

	std::vector<int> colors;
	std::vector<float> posX;
	std::vector<float> posY;
	std::vector<float> velX;
	std::vector<float> velY;
};

inline Rgb lerp(const Rgb& lhs, const Rgb& rhs, float t)
{
	t = (t + 1) / 2;
	const float r = (1.0f - t) * lhs.r + t * rhs.r;
	const float g = (1.0f - t) * lhs.g + t * rhs.g;
	const float b = (1.0f - t) * lhs.b + t * rhs.b;
	return Rgb{ r, g, b };
}
#pragma once
#include <cstdlib>

inline float frand() {
	constexpr int max = 10000;
	return static_cast<float>(rand() % max) / max;
}

inline float warp(float x)
{
	if (x < 0)
		return x + 1.0f;
	if (x > 1)
		return x - 1.0f;
	return x;
}

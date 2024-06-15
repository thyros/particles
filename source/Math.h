#pragma once
#include <cstdlib>
#include <random>

inline float frand() {
	constexpr int max = 10000;
	return static_cast<float>(rand() % max) / max;
}

inline float rand(float min, float max) {
	std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<> dist(min, max);

    return static_cast<float>(dist(rng));
}

inline float warp(float x)
{
	if (x < 0)
		return x + 1.0f;
	if (x > 1)
		return x - 1.0f;
	return x;
}

inline float map(float f, float left, float right, float startValue, float endValue) {
	if (f < left) {
		return startValue;
	} 
	
	if (f > right) {
		return endValue;
	}

	const float len = right - left;
	const float factor = (f - left) / len;
	return startValue + (endValue - startValue) * factor;
}

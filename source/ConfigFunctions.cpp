#include "ConfigFunctions.h"
#include "math.h"
#include <cassert>

Matrix generateMatrix(const int m, std::function<float(int r, int c)> generator) {
	Matrix matrix = std::vector(m, std::vector(m, 0.0f));
	for (int r = 0; r < m; ++r) {
		for (int c = 0; c < m; ++c) {
			matrix[r][c] = generator(r, c);
		}
	}
	return matrix;
}

Matrix generateRandomMatrix(const int m)
{
	return generateMatrix(m, [](int, int) -> float { return frand() * 2 - 1;});
}

Matrix generateIdentityMatrix(const int m)
{
	return generateMatrix(m, [](int r, int c) -> float { return r == c ? 1 : 0; });
}

Matrix generateForces(const int m) {
	return generateMatrix(m, [](int, int) -> float {
		float f = rand(0.3f, 1.0f);
			if (rand(0.0f, 100.0f) < 50) {
				f *= -1;
			}
		return f;
	});
}

Matrix generateDistances (const int m) {
	return generateMatrix(m, [](int, int) -> float {
		return rand(30, 50);
	});
}

Matrix generateRadii (const int m) {
	return generateMatrix(m, [](int, int) -> float {
		return rand(60, 250);
	});
}


ParticleColors generateRandomColors(const int c)
{
	const Rgb rgbs[] = {ToRgb(255, 0, 0),
						ToRgb(0, 255, 0),
						ToRgb(0, 0, 255),
						ToRgb(93, 138, 168),
						ToRgb(164, 198, 57),
						ToRgb(205, 149, 117),
						ToRgb(253, 238, 0),
						ToRgb(138, 43, 226),
						ToRgb(102, 255, 0),
						ToRgb(237, 135, 45),
						ToRgb(128, 128, 0),
						ToRgb(165, 11, 94)};
	assert(c < sizeof rgbs);

	return ParticleColors(std::begin(rgbs), std::end(rgbs) + c);
}

Rgb ToRgb(int r_, int g_, int b_)
{
	assert(r_ >= 0 && r_ <= 255);
	assert(g_ >= 0 && g_ <= 255);
	assert(b_ >= 0 && b_ <= 255);
	return Rgb{.r = static_cast<float>(static_cast<double>(r_) / 255), .g = static_cast<float>(static_cast<double>(g_) / 255), .b = static_cast<float>(static_cast<double>(b_) / 255)};
}

Rgb lerp(const Rgb &lhs, const Rgb &rhs, float t)
{
	t = (t + 1) / 2;
	const float r = (1.0f - t) * lhs.r + t * rhs.r;
	const float g = (1.0f - t) * lhs.g + t * rhs.g;
	const float b = (1.0f - t) * lhs.b + t * rhs.b;
	return Rgb{.r = r, .g = g, .b = b};
}
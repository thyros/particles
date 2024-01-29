#pragma once
#include <cmath>

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

	float magnitude() const
	{
		return std::sqrt(x * x + y * y);
	}

	void normalize() {
		float m = magnitude();

		if (m > 0) {
			div(m);
		}
	}

	float distance(const Vec& v) const
	{
		const float dx = x - v.x;
		const float dy = y - v.y;
		const float dist = std::sqrt(dx * dx + dy * dy);
		return dist;
	}
};

inline Vec sub(const Vec& v1, const Vec& v2) {
	return Vec{ v1.x - v2.x, v1.y - v2.y };
}

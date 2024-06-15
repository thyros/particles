#pragma once
#include "Math.h"
#include <cmath>

struct Vec {
	float x = 0.0f;
	float y = 0.0f;

	Vec& add(const Vec& v) {
		x += v.x;
		y += v.y;
		return *this;
	}

	Vec& sub(const Vec& v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}

	Vec& div(float f) {
		x /= f;
		y /= f;
		return *this;
	}

	Vec& mul(float f) {
		x *= f;
		y *= f;
		return *this;
	}

	float magnitude() const
	{
		return std::sqrt(x * x + y * y);
	}

	Vec& normalize() {
		float m = magnitude();

		if (m > 0) {
			div(m);
		}

		return *this;
	}

	float distance(const Vec& v) const
	{
		const float dx = x - v.x;
		const float dy = y - v.y;
		const float dist = std::sqrt(dx * dx + dy * dy);
		return dist;
	}
};

inline Vec operator+(const Vec& lhs, const Vec& rhs) {
	return Vec{.x = lhs.x + rhs.x, .y = lhs.y + rhs.y };
}

inline Vec operator-(const Vec& lhs, const Vec& rhs) {
	return Vec{.x = lhs.x - rhs.x, .y = lhs.y - rhs.y };
}

inline Vec wrap(Vec v, Vec lim) {
	if (v.x < 0) {
		v.x += lim.x;
	} else if (v.x > lim.x) {
		v.x -= lim.x;
	}
	if (v.y < 0) {
		v.y += lim.y;
	} else if (v.y > lim.y) {
		v.y -= lim.y;
	}
	return v;
}

inline Vec randomVec() {
	return Vec {.x = rand(0.0f, 1.0f), .y = rand(0.0f, 1.0f)};
}
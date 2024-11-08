#pragma once
#include "Vec.h"
#include <vector>

struct Position {
	float x;
	float y;
};

struct Velocity {
	float x;
	float y;
};

struct State
{
	std::vector<int> colors;
	std::vector<Position> pos;
	std::vector<Velocity> vel;
};


inline void AddParticle(State& state, const float x, const float y, const int c) {
	state.pos.push_back(Position{.x = x, .y = y});
	state.vel.push_back(Velocity{});
	state.colors.push_back(c);
}
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



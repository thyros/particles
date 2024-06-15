#pragma once
#include "Vec.h"
#include <vector>

struct State
{
	std::vector<int> colors;
	std::vector<Vec> pos;
	std::vector<Vec> vel;
};



#pragma once

#include "Config.h"
#include <functional>

Matrix generateMatrix(const int m, std::function<float(int r, int c)> generator);
Matrix generateRandomMatrix(const int m);
Matrix generateIdentityMatrix(const int m);
ParticleColors generateRandomColors(const int c);

Matrix generateForces(const int m);
Matrix generateDistances (const int m);
Matrix generateRadii (const int m);

/// Creates the Rgb structure using r, g, b values in range [0, 255]
Rgb ToRgb(int r_, int g_, int b_);


Rgb lerp(const Rgb &lhs, const Rgb &rhs, float t);

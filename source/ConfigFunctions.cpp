#include "ConfigFunctions.h"

Matrix generateRandomMatrix(const int m)
{
	Matrix matrix = std::vector(m, std::vector(m, 0.0f));

	for (int i = 0; i < m; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			const float r = frand() * 2 - 1;
			matrix[i][j] = r;
		}
	}

	return matrix;
}

Matrix generateIdentityMatrix(const int m)
{
	Matrix matrix = std::vector(m, std::vector(m, 0.0f));

	for (int i = 0; i < m; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			if (i == j)
				matrix[i][j] = 1;
		}
	}

	return matrix;
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
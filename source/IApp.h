#pragma once
#include "State.h"
#include <memory>

class IApp
{
public:
	virtual ~IApp() = default;

	virtual bool Update() = 0;
	virtual void Render() = 0;

	virtual void DrawParticle(float x, float y, int size, int color) = 0;
};

std::unique_ptr<IApp> CreateApp(State& state, int16_t width, int16_t height);

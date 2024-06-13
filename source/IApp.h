#pragma once
#include "State.h"
#include <memory>

class IApp
{
public:
	virtual ~IApp() = default;

	virtual void Run() = 0;
};

std::unique_ptr<IApp> CreateApp(Config& config, State& state, int16_t width, int16_t height);

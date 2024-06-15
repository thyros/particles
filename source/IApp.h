#pragma once
#include "Config.h"
#include "State.h"
#include <memory>
#include <cstdint>

class IApp
{
public:
	virtual ~IApp() = default;

	virtual void Run() = 0;
};

std::unique_ptr<IApp> CreateApp(Config& config, State& state, int16_t width, int16_t height);

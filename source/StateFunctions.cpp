#include "StateFunctions.h"
#include "Math.h"

State generateRandomState(int particlesCount, int colorsCount, int width, int height)
{
    State state;
    for (int i = 0; i < particlesCount; ++i)
    {
        state.colors.push_back(rand() % colorsCount);
        state.pos.push_back(Vec {.x = rand(0, width), .y = rand(0, height)} );
        state.vel.push_back(Vec {});
    }
    return state;
}

State generateAllInTheMiddleState(int particlesCount, int colorsCount, int width, int height) {
    State state;
    for (int i = 0; i < particlesCount; ++i) {
        state.colors.push_back(rand() % colorsCount);
        state.pos.push_back(Vec {.x = static_cast<float>(width) / 2, .y = static_cast<float>(height) / 2});
        state.vel.push_back(Vec {});
    }
    return state;
}
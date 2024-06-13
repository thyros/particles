#include "StateFunctions.h"
#include "Math.h"

State generateState(int particlesCount, int colorsCount)
{
    State state;
    for (int i = 0; i < particlesCount; ++i)
    {
        state.colors.push_back(rand() % colorsCount);
        state.posX.push_back(frand());
        state.posY.push_back(frand());
        state.velX.push_back(0);
        state.velY.push_back(0);
    }
    return state;
}

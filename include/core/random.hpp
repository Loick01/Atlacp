#pragma once

#include "core/core_types.hpp" // Direction

struct Random
{
    Random()
    {
        srand(time(NULL));
    }

    float GetRandomFloat(float min, float max) const
    {
        return ((float)rand()/RAND_MAX)*(max-min)+min;
    }

    Direction GetRandomDirection() const
    {
        return static_cast<Direction>(rand()%4);
    }
};
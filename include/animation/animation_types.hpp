#pragma once

#include <vector>

#include "core/core_types.hpp" // Vec2

struct AnimationData
{
    std::vector<Vec2> sprites; // Position of each sprites in spritesheet. Should use ScreenPosition (or something else) instead of Vec2 ?
    AreaSize spriteSize;
    int step; // How many step for the animation
    float frameDuration;
};
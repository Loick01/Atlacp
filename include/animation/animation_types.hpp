#pragma once

#include <vector>

#include "core/core_types.hpp" // Vec2

struct SpriteAnimationData
{
    std::vector<Vec2> sprites; // Position of each sprites in spritesheet
    AreaSize spriteSize;
    int step; // How many step for the animation
    float frameDuration;
};
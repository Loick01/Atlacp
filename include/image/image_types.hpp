#pragma once

#include <string>

#include "core/core_types.hpp" // Vec2

using FontKey = std::string;
using TextureKey = std::string;

enum class FontSize
{
    Small // Will have Medium, High, etc.
};

struct ScenePosition : public Vec2
{
    ScenePosition() = default;
    ScenePosition(const int px, const int py) { x = px; y = py; }
    ScenePosition(const Vec2& v) { x = v.x; y = v.y; }
    ScenePosition(const Vec2f& v) { x = v.x; y = v.y; }
};

struct ScreenPosition : public Vec2
{
    ScreenPosition() = default;
    ScreenPosition(const int px, const int py) { x = px; y = py; }
    ScreenPosition(const float px, const float py) { x = px; y = py; }
    ScreenPosition(const Vec2& v) { x = v.x; y = v.y; }
};
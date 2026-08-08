#pragma once

#include <vector>

#include "core/core_types.hpp" // Vec2
#include "image/image_types.hpp" // ScenePosition
#include "map/interaction/order.hpp"

struct MapPosition : public Vec2 // Rename GridPosition ?
{
    MapPosition() = default;
    MapPosition(const int px, const int py) { x = px; y = py; }
    MapPosition(const Vec2& v) { x = v.x; y = v.y; }

    ScenePosition ToScenePosition(const int tileSize) const
    {
        return ScenePosition{x, y}*tileSize;
    }

    unsigned int GetManhattanDistance(const MapPosition& mp) const
    {
        return std::abs(x - mp.x) + std::abs(y - mp.y);
    }
};

enum class MapBehaviour
{
    Random, Follow, GoTo
};

enum class MapBound
{
    Inside,
    OutUp,
    OutDown,
    OutRight,
    OutLeft
};

struct DataNPC // Rename ?
{
    std::vector<Order> orders;
    MapPosition position;
    std::string sprite;
    float walkSpeed;
    float runSpeed;
};

struct DataMapElement // Rename ?
{
    std::vector<Order> orders;
    MapPosition position;
};
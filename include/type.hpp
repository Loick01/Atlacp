#pragma once

#include <cstdint>
#include <set>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <vector>    

using TextureKey = std::string;
using Tile = unsigned int; // Use a struct ?

// Operator signature list : https://gist.github.com/beached/38a4ae52fcadfab68cb6de05403fa393

// Should create a Size struct ?

template <typename T>
struct Pair
{
    T x;
    T y;
};

struct Vec2 : public Pair<int>
{
    Vec2 operator+(const Vec2 rhs) const
    {
        return {x+rhs.x, y+rhs.y};
    }

    Vec2 operator-(const Vec2 rhs) const
    {
        return {x-rhs.x, y-rhs.y};
    }

    Vec2 operator+(const int rhs) const
    {
        return {x+rhs, y+rhs};
    }

    Vec2& operator+=(const Vec2& rhs) 
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vec2 operator*(const Pair<int> rhs) const
    {
        return {x*rhs.x, y*rhs.y};
    }

    Vec2 operator*(const float rhs) const
    {
        return {static_cast<int>(x*rhs), static_cast<int>(y*rhs)};
    }

    Vec2 operator/(const float rhs) const
    {
        return {static_cast<int>(x/rhs), static_cast<int>(y/rhs)};
    }
};

struct ScenePosition : public Vec2
{
    ScenePosition() = default;
    ScenePosition(const int px, const int py) { x = px; y = py; }
    ScenePosition(const Vec2& v) { x = v.x; y = v.y; }

    Pair<bool> operator>(const Vec2 rhs) const
    {
        return {x>rhs.x, y>rhs.y};
    }
};

struct ScreenPosition : public Vec2
{
    ScreenPosition() = default;
    ScreenPosition(const int px, const int py) { x = px; y = py; }
    ScreenPosition(const Vec2& v) { x = v.x; y = v.y; }
};

// Rename GridPosition ?
struct MapPosition : public Vec2
{
    MapPosition() = default;
    MapPosition(const int px, const int py) { x = px; y = py; }
    MapPosition(const Vec2& v) { x = v.x; y = v.y; }

    ScenePosition ToScenePosition(const int tile_size) const
    {
        return ScenePosition{x*tile_size, y*tile_size};
    }
};

enum class MapBound
{
    Inside,
    OutUp,
    OutDown,
    OutRight,
    OutLeft
};

enum class MapDirection
{
    // Do not change the order (sprite animation is based on implicit int defined in this enum)
    // Random::GetRandomDirection also use this order
    Down, // 0
    Right, // 1
    Up, // 2
    Left, // 3
    None
};

struct WorldData
{
    std::vector<std::string> maps;
    int width;
    int height;
};

struct MapData
{
    std::vector<Tile> map;
    std::vector<TextureKey> tilesets; // Tileset keys (read from the header of the map file) that will be used to build the map
    MapPosition spawn_position; // Position where the player will start when loading the associated map (-1 if no specific position)
    int width;
    int height;
};

struct TilesetData
{
    TextureKey tileset_key;
    std::set<Tile> solid_tiles; // Should use unordered set ?
    int width;
    int height;
    int tile_size;
};

struct AnimationData
{
    std::vector<Vec2> sprites; // Position (column, line) of each sprites in spritesheet
    Pair<int> sprite_size;
    int step; // How many step for the animation
    float frame_duration;
};

struct TilesetNormalizationInfo
{
    int last_lower_bound;
    int last_upper_bound;
};

// Should not be here ?
struct Random
{
    Random()
    {
        srand(time(NULL));
    }

    MapDirection GetRandomDirection() const
    {
        return static_cast<MapDirection>(rand()%4);
    }
};
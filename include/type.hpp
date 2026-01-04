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

template <typename T>
struct Pair
{
    T x;
    T y;
};

struct Vec2 : public Pair<int>
{
    bool operator==(const Vec2& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }

    bool operator!=(const Vec2& rhs ) const
    {
        return x != rhs.x || y != rhs.y;
    }

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

    Pair<bool> operator>(const Vec2 rhs) const
    {
        return {x>rhs.x, y>rhs.y};
    }
};

using AreaSize = Vec2;
using GridSize = Vec2;

template <typename T>
std::ostream& operator<<(std::ostream& os, const Pair<T>& v) {
    os << "(x=" << v.x << ", y=" << v.y << ")\n";
    return os;
}

struct ScenePosition : public Vec2
{
    ScenePosition() = default;
    ScenePosition(const int px, const int py) { x = px; y = py; }
    ScenePosition(const Vec2& v) { x = v.x; y = v.y; }
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
        return ScenePosition{x, y}*tile_size;
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
    GridSize size;
    size_t start_map;
};

struct TileLayer
{
    std::vector<Tile> tiles;
};

struct MapData
{
    size_t layer_count;
    std::vector<TileLayer> map;
    std::vector<bool> occupancy_grid; // Could be a TileLayer ?
    std::vector<TextureKey> tilesets; // Tileset keys (read from the header of the map file) that will be used to build the map
    MapPosition spawn_position; // Position where the player will start when loading the associated map (-1 if no specific position)
    GridSize size;
};

struct TilesetData
{
    TextureKey tileset_key;
    std::set<Tile> solid_tiles; // Should use unordered set ?
    GridSize size;
    int tile_size;
};

struct AnimationData
{
    std::vector<Vec2> sprites; // Position of each sprites in spritesheet. Should use ScreenPosition (or something else) instead of Vec2 ?
    AreaSize sprite_size;
    int step; // How many step for the animation
    float frame_duration;
};

// Remove ?
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

    float GetRandomFloat(float min, float max) const
    {
        return ((float)rand()/RAND_MAX)*(max-min)+min;
    }

    MapDirection GetRandomDirection() const
    {
        return static_cast<MapDirection>(rand()%4);
    }
};
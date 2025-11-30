#pragma once

#include <cstdint>
#include <set>
#include <string>
#include <vector>

using TextureKey = std::string;
using Tile = unsigned int; // Use a struct ?

// Operator signature list : https://gist.github.com/beached/38a4ae52fcadfab68cb6de05403fa393

// Should create a Dimension struct

template <typename T>
struct Pair // Should merge with Vec2 ?
{
    T x;
    T y;

    Pair<T> operator*(const Pair<T> rhs) const
    {
        return Pair<T>{x*rhs.x, y*rhs.y};
    }
};

template <typename T>
struct Vec2
{
    int x;
    int y;

    T operator+(const T& rhs) const
    {
        return T{x+rhs.x, y+rhs.y};
    }
    
    T& operator+=(const T& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return static_cast<T&>(*this);
    }

    T operator-(const T& rhs) const
    {
        return T{x-rhs.x, y-rhs.y};
    }

    T operator*(const float rhs) const
    {
        return T{static_cast<int>(x*rhs), static_cast<int>(y*rhs)};
    }

    T operator+(const int rhs) const
    {
        return T{x+rhs, y+rhs};
    }

    T operator/(const int rhs) const
    {
        return T{x/rhs, y/rhs};
    }

    Pair<bool> operator>(const T& rhs) const
    {
        return Pair<bool>{x>rhs.x, y>rhs.y};
    }
};

// CRTP
struct ScreenPosition : Vec2<ScreenPosition> // Position on screen (could be out of window)
{

};

// CRTP
struct ScenePosition : Vec2<ScenePosition> // Position in 2D space
{
    using Vec2<ScenePosition>::operator+; // Used for ScenePosition + ScenePosition
    using Vec2<ScenePosition>::operator-; // Used for ScenePosition - ScenePosition

    ScenePosition operator+(const ScreenPosition rhs) const // Hide Vec2::operator+ (should not be in ScenePosition ?)
    {
        return ScenePosition{x+rhs.x, y+rhs.y};
    }

    ScenePosition operator-(const ScreenPosition rhs) const // Hide Vec2::operator- (should not be in ScenePosition ?)
    {
        return ScenePosition{x-rhs.x, y-rhs.y};
    }
};

// CRTP
struct MapPosition : Vec2<MapPosition>
{
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
    Up,
    Down,
    Right,
    Left,
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

struct TilesetNormalizationInfo
{
    int last_lower_bound;
    int last_upper_bound;
};
#pragma once

#include <vector>
#include <set>
#include <string>

using TextureKey = std::string;

// Operator signature list : https://gist.github.com/beached/38a4ae52fcadfab68cb6de05403fa393

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
};

// CRTP
struct ScreenPosition : Vec2<ScreenPosition> // Position on screen (could be out of window)
{

};

// CRTP
struct ScenePosition : Vec2<ScenePosition> // Position in 2D space
{
    // using Vec2<ScenePosition>::operator+;

    ScenePosition operator+(const ScreenPosition rhs) const // Hide Vec2::operator+ (should not be in ScenePosition ?)
    {
        return ScenePosition{x+rhs.x, y+rhs.y};
    }
};

// CRTP
struct MapPosition : Vec2<MapPosition>
{

};

enum class MapBound
{
    Inside,
    OutUp,
    OutDown,
    OutRight,
    OutLeft
};

enum class MapMovement
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
    std::vector<unsigned char> map; // Should define Tile type instead of using unsigned char ?
    std::vector<TextureKey> tilesets; // Tileset keys (read from the header of the map file) that will be used to build the map
    int width;
    int height;
};

struct TilesetData
{
    TextureKey tileset_key;
    std::set<unsigned char> solid_tiles; // Should use unordered set ?
    int width;
    int height;
    int tile_size;
};

struct TilesetNormalizationInfo
{
    int last_lower_bound;
    int last_upper_bound;
};
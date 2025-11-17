#pragma once

#include <vector>
#include <set>
#include <string>

using TextureKey = std::string;

struct ScreenPosition // Position on screen (could be out of window)
{
    int x;
    int y;

    ScreenPosition operator+(const ScreenPosition p) const
    {
        return ScreenPosition{x+p.x, y+p.y};
    }

    ScreenPosition operator+(const int i) const
    {
        return ScreenPosition{x+i, y+i};
    }

    ScreenPosition operator-(const ScreenPosition p) const
    {
        return ScreenPosition{x-p.x, y-p.y};
    }
};

struct ScenePosition // Position in 2D space
{
    int x;
    int y;

    ScenePosition operator+(const ScenePosition p) const
    {
        return ScenePosition{x+p.x, y+p.y};
    }

    ScenePosition operator+(const int i) const
    {
        return ScenePosition{x+i, y+i};
    }

    ScenePosition operator-(const ScenePosition p) const
    {
        return ScenePosition{x-p.x, y-p.y};
    }

    ScenePosition operator+(const ScreenPosition p) const
    {
        return ScenePosition{x+p.x, y+p.y};
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

enum class MapMovement
{
    Up,
    Down,
    Right,
    Left,
    None
};

struct MapPosition
{
    int x;
    int y;

    MapPosition operator+(const MapPosition p) const
    {
        return MapPosition{x+p.x, y+p.y};
    }

    MapPosition operator*(const int i) const
    {
        return MapPosition{x*i, y*i};
    }

    ScreenPosition ToScreenPosition(const int tile_size) const
    {
        return ScreenPosition{x*tile_size, y*tile_size};
    }
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
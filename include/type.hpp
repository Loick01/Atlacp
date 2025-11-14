#pragma once

#include <vector>
#include <set>
#include <string>

using TextureKey = std::string;

struct ScreenPosition
{
    int x;
    int y;

    ScreenPosition operator-(const ScreenPosition p) const
    {
        return ScreenPosition{x-p.x, y-p.y};
    }
};

enum class MapBound
{
    Inside,
    OutTop,
    OutBottom,
    OutRight,
    OutLeft
};

struct MapPosition
{
    int x;
    int y;

    MapPosition operator+(const MapPosition p) const
    {
        return MapPosition{x+p.x, y+p.y};
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
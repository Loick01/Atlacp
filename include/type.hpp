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

struct MapPosition
{
    int x;
    int y;

    MapPosition operator+(const MapPosition p) const
    {
        return MapPosition{x+p.x, y+p.y};
    }
};

struct MapData
{
    std::vector<unsigned char> map; // Should define Tile type instead of using unsigned char ?
    unsigned int width;
    unsigned int height;
};

struct TilesetData
{
    std::set<unsigned char> solid_tiles; // Should use unordered set ?
    unsigned int width;
    unsigned int height;
    unsigned int tile_size;
};
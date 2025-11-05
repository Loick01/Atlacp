#pragma once

#include <vector>

struct Position
{
    unsigned char x; // map_width is also unsigned char
    unsigned char y;
};

struct MapData
{
    std::vector<unsigned char> map; // Should define Tile type instead of using unsigned char ?
    unsigned char width;
    unsigned char height;
    unsigned char tile_size;
};
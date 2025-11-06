#pragma once

#include <vector>

struct Position
{
    int x;
    int y;

    Position operator+(Position p) const
    {
        return Position{x+p.x, y+p.y};
    }
};

struct MapData
{
    std::vector<unsigned char> map; // Should define Tile type instead of using unsigned char ?
    unsigned char width;
    unsigned char height;
    unsigned char tile_size;
};
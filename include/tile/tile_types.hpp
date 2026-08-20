#pragma once

#include <set>
#include <vector>

#include "core/core_types.hpp" // GridSize
#include "image/image_types.hpp" // TextureKey
#include "tile/tile_types.hpp" // Tile

using Tile = unsigned int; // Use a struct ?

struct WorldData
{
    std::vector<std::string> maps;
    std::string mapDirectoryPath;
    std::string npcsFile;
    std::string spritePlayerPath;
    GridSize size;
    size_t startMap;
};

struct TilesetData
{
    TextureKey tilesetKey;
    std::set<Tile> solidTiles; // Should use unordered set ?
    GridSize size;
    int tileSize;
};

struct TilesetNormalizationInfo
{
    int lastLowerBound;
    int lastUpperBound;
};
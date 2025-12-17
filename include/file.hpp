#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "type.hpp"

#define MAP_HEADER_END "###"

class FileReader
{
    private:
        void ReadHeaderMapFile(std::ifstream& input, MapData& m) const;

    public:
        FileReader() = default;

        WorldData ReadWorldFile(const std::string& world_filepath) const;
        MapData GetMapFromFile(const std::string& path) const;
        TilesetData GetTilesetFromFile(const std::string& path) const;
        AnimationData GetAnimationFromFile(const std::string& path) const;
        void SaveMapFile(const std::string& map_filepath, const MapData& map_data) const;
};
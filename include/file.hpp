#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "layer.hpp"
#include "type.hpp"

#define MAP_HEADER_END "###" // Will be removed ?

class FileReader
{
    private:
        void ReadHeaderMapFile(std::ifstream& input, MapData& m) const;

    public:
        FileReader() = default;

        WorldData ReadWorldFile(const std::string& world_filepath) const;
        MapData GetMapFromFile(const std::string& path, Camera& camera, TextureController& texture_controller,
            Tileset& tileset, const bool layer_culling) const;
        TilesetData GetTilesetFromFile(const std::string& path) const;
        AnimationData GetAnimationFromFile(const std::string& path) const;
        void SaveMapFile(const std::string& map_filepath, const MapData& map_data) const;
};
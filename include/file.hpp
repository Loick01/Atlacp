#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "layer.hpp"
#include "type.hpp"

#define MAP_HEADER_END "###" // Will be removed ? Rename (I also use it for NPC file)

class FileReader
{
    private:
        void ReadHeaderMapFile(std::ifstream& input, MapData& m) const;

    public:
        FileReader() = default;

        std::vector<DataNPC> ReadDataNPCs(const std::string& npcsFilepath, const unsigned int mapIndex) const;
        WorldData ReadWorldFile(const std::string& worldFilepath) const;
        MapData GetMapFromFile(const std::string& path, Camera& camera, TextureController& textureController,
            Tileset& tileset) const;
        TilesetData GetTilesetFromFile(const std::string& path) const;
        AnimationData GetAnimationFromFile(const std::string& path) const;
        void SaveMapFile(const std::string& mapFilepath, const MapData& mapData) const;
};
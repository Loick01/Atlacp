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
        FileReader();
        ~FileReader();

        std::vector<std::string> ReadWorldFile(const std::string& world_filepath, int& world_width, int& world_height) const;
        void GetMapFromFile(const std::string& filepath, MapData& data) const;
        void ReadTilesetHeader(const std::string& tileset_header, TilesetData& m_map) const;
        void SaveMapFile(const std::string& map_filepath, const MapData& map_data) const;
};
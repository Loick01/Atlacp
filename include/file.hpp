#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "type.hpp"

class FileReader
{
    private:
        void ReadHeaderMapFile(std::ifstream& input, MapData& m) const;

    public:
        FileReader();
        ~FileReader();

        void GetMapFromFile(const std::string& filepath, MapData& data) const;
        void GetInfoFromTileset(const std::string& tileset_header, TilesetData& m_map) const;
};
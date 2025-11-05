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

        MapData GetMapFromFile(const std::string& filepath) const;
};
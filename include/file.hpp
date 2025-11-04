#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <stdint.h>

class FileReader
{
    private:
        std::ifstream m_input_file;
        void ReadHeaderMapFile(unsigned char& map_width, unsigned char& map_height, unsigned char& tile_size);

    public:
        FileReader();
        ~FileReader();

        void OpenFile(const std::string& filepath);
        void CloseFile();
        std::vector<unsigned char> GetMapFromFile(const std::string& filepath, unsigned char& map_width, unsigned char& map_height, unsigned char& tile_size);
};
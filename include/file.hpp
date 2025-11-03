#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class FileReader
{
    private:
        std::ifstream m_input_file;
    public:
        FileReader();
        ~FileReader();

        void OpenFile(const char* filepath);
        void CloseFile();
        std::vector<unsigned int> GetMapFromFile(const char* filepath, unsigned int& map_width, unsigned int& map_height, unsigned int& tile_size);
};
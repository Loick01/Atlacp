#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class FileReader
{
    private:
        std::ifstream m_input_file;
        std::string m_current_line;
    public:
        FileReader();
        ~FileReader();

        void OpenFile(const char* filepath);
        void CloseFile();
        std::vector<unsigned char> GetMapFromFile(const char* filepath);
};
#include "file.hpp"

FileReader::FileReader()
{

}

FileReader::~FileReader()
{

}

void FileReader::OpenFile(const char* filepath)
{
    m_input_file.open(filepath);
    if (!m_input_file.is_open()) std::cout << "Failed to open this file : " << filepath << "\n";
}

void FileReader::CloseFile()
{
    m_input_file.close();
}

std::vector<unsigned char> FileReader::GetMapFromFile(const char* filepath)
{
    OpenFile(filepath);
    std::vector<unsigned char> map;
    unsigned int map_width;
    unsigned int map_height;
    m_input_file >> map_width;
    m_input_file >> map_height;
    map.reserve(map_width*map_height);
    
    unsigned char current_value;
    while (m_input_file >> current_value){
        map.push_back(current_value);
    }

    CloseFile();
    return map;
}
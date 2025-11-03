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

std::vector<unsigned int> FileReader::GetMapFromFile(const char* filepath, unsigned int& map_width, unsigned int& map_height, unsigned int& tile_size)
{
    OpenFile(filepath);
    std::vector<unsigned int> map;
    m_input_file >> map_width;
    m_input_file >> map_height;
    m_input_file >> tile_size;
    map.reserve(map_width*map_height);
    
    unsigned int current_value;
    while (m_input_file >> current_value){
        map.push_back(current_value);
    }

    CloseFile();
    return map;
}
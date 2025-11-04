#include "file.hpp"

FileReader::FileReader()
{

}

FileReader::~FileReader()
{

}

void FileReader::OpenFile(const std::string& filepath)
{
    m_input_file.open(filepath);
    if (!m_input_file.is_open()) std::cout << "Failed to open this file : " << filepath << "\n";
}

void FileReader::CloseFile()
{
    m_input_file.close();
}

void FileReader::ReadHeaderMapFile(unsigned char& map_width, unsigned char& map_height, unsigned char& tile_size)
{
    unsigned int v;
    m_input_file >> v; map_width = static_cast<unsigned char>(v);
    m_input_file >> v; map_height = static_cast<unsigned char>(v);
    m_input_file >> v; tile_size = static_cast<unsigned char>(v);
}

std::vector<unsigned char> FileReader::GetMapFromFile(const std::string& filepath, unsigned char& map_width, unsigned char& map_height, unsigned char& tile_size)
{
    OpenFile(filepath);
    ReadHeaderMapFile(map_width, map_height, tile_size);
    
    std::vector<unsigned char> map;
    map.reserve(map_width*map_height);
    
    unsigned int current_value;
    while (m_input_file >> current_value){
        map.push_back(static_cast<unsigned char>(current_value));
    }

    CloseFile();
    return map;
}
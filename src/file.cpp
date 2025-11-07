#include "file.hpp"

FileReader::FileReader()
{

}

FileReader::~FileReader()
{

}

void FileReader::ReadHeaderMapFile(std::ifstream& input, MapData& m) const
{
    unsigned int v;
    input >> v; m.width = v;
    input >> v; m.height = v;
    input >> v; m.tile_size = v;
}

MapData FileReader::GetMapFromFile(const std::string& filepath) const
{
    std::ifstream input;
    input.open(filepath);
    
    MapData data;
    ReadHeaderMapFile(input, data);
    data.map.reserve(data.width*data.height);
    
    unsigned int current_value;
    while (input >> current_value){
        data.map.push_back(static_cast<unsigned char>(current_value));
    }

    input.close();
    return data;
}
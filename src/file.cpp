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
    std::string s;
    while (input >> s && s != "###"){
        m.tilesets.push_back(s);
    }
}

void FileReader::GetMapFromFile(const std::string& filepath, MapData& data) const
{
    std::ifstream input;
    input.open(filepath);
    
    ReadHeaderMapFile(input, data);
    data.map.clear();
    data.map.reserve(data.width*data.height);
    
    unsigned int current_value;
    while (input >> current_value){
        data.map.push_back(current_value);
    }

    input.close();
}

void FileReader::ReadTilesetHeader(const std::string& tileset_header, TilesetData& data) const
{
    std::ifstream input;
    input.open(tileset_header);

    unsigned int v;
    input >> data.width;
    input >> data.height;
    input >> data.tile_size;

    data.solid_tiles.clear();
    while (input >> v){
        data.solid_tiles.insert(v);
    }
}
#include "file.hpp"

FileReader::FileReader()
{

}

FileReader::~FileReader()
{

}

std::vector<std::string> FileReader::ReadWorldFile(const std::string& world_filepath, int& world_width, int& world_height) const
{
    std::ifstream input;
    input.open(world_filepath);

    int v;
    input >> v; world_width = v;
    input >> v; world_height = v;
    
    std::vector<std::string> maps;
    std::string s;
    while (input >> s)
        maps.push_back(s);

    input.close();
    return maps;
}

void FileReader::ReadHeaderMapFile(std::ifstream& input, MapData& m) const
{
    int v;
    input >> v; m.width = v;
    input >> v; m.height = v;
    input >> v; m.spawn_position.x = v;
    input >> v; m.spawn_position.y = v;
    std::string s;
    while (input >> s && s != MAP_HEADER_END){
        m.tilesets.push_back(s);
    }
}

void FileReader::GetMapFromFile(const std::string& filepath, MapData& data) const
{
    std::ifstream input;
    input.open(filepath);
    
    data.tilesets.clear();
    data.map.clear();
    ReadHeaderMapFile(input, data);
    data.map.reserve(data.width*data.height);
    
    Tile current_value;
    while (input >> current_value){
        data.map.push_back(current_value);
    }

    input.close();
}

void FileReader::ReadTilesetHeader(const std::string& tileset_header, TilesetData& data) const
{
    std::ifstream input;
    input.open(tileset_header);

    int v;
    input >> data.width;
    input >> data.height;
    input >> data.tile_size;

    //data.solid_tiles.clear();
    while (input >> v){
        data.solid_tiles.insert(v);
    }
}

void FileReader::SaveMapFile(const std::string& map_filepath, const MapData& map_data) const
{
    std::ofstream map_file(map_filepath);
    int width = map_data.width;
    int height = map_data.height;
    MapPosition spawn = MapPosition{-1, -1}; // Later, make something to select in editor the spawning tile
    // Need to check if a file with the given name already exist

    // Header
    map_file << width << " " << height << " " << spawn.x << " " << spawn.y << std::endl;
    for (const TextureKey& k : map_data.tilesets){ // Tileset filepath must be write in order
        map_file << k << std::endl; // TilesetKey need definition for operator<<
    }
    map_file << MAP_HEADER_END << std::endl;
    
    // Map
    for (size_t j = 0 ; j < height ; j++){
        for (size_t i = 0 ; i < width ; i++){
            map_file << map_data.map[j*width+i] << " ";
        }
        map_file << std::endl;
    }

    map_file.close();
}
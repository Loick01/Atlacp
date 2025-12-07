#include "file.hpp"

FileReader::FileReader()
{

}

FileReader::~FileReader()
{

}

WorldData FileReader::ReadWorldFile(const std::string& world_filepath) const
{
    std::ifstream input;
    input.open(world_filepath);
    WorldData data;
    
    int v;
    input >> v; data.width = v;
    input >> v; data.height = v;
    
    std::string s;
    data.maps.reserve(data.width*data.height);
    while (input >> s)
        data.maps.push_back(s);

    input.close();
    return data;
}

void FileReader::ReadHeaderMapFile(std::ifstream& input, MapData& data) const
{
    int v;
    input >> v; data.width = v;
    input >> v; data.height = v;
    input >> v; data.spawn_position.x = v;
    input >> v; data.spawn_position.y = v;
    std::string s;
    while (input >> s && s != MAP_HEADER_END)
        data.tilesets.push_back(s);
}

MapData FileReader::GetMapFromFile(const std::string& path) const
{
    std::ifstream input;
    const std::string map_filepath = "../assets/maps/" + path; // Create a function in File
    input.open(map_filepath);
    MapData data;

    ReadHeaderMapFile(input, data);
    data.map.reserve(data.width*data.height);
    
    Tile current_value;
    while (input >> current_value){
        data.map.push_back(current_value);
    }

    input.close();
    return data;
}

AnimationData FileReader::GetAnimationFromFile(const std::string& path) const
{
    std::ifstream input;
    input.open(path);
    AnimationData data;

    input >> data.sprite_size.x;
    input >> data.sprite_size.y;
    input >> data.step;
    input >> data.frame_duration;

    int v1, v2;
    data.sprites.reserve(data.step*4);
    while (input >> v1 && input >> v2) // Use a for loop with data.step instead ?
        data.sprites.push_back(Vec2{v1, v2}*data.sprite_size);

    input.close();
    return data;
}

TilesetData FileReader::GetTilesetFromFile(const std::string& path) const
{
    std::ifstream input;
    const std::string tileset_filepath = "../assets/tilesets/" + path; // Create a function in File
    input.open(tileset_filepath);
    TilesetData data;

    input >> data.width;
    input >> data.height;
    input >> data.tile_size;

    int v;
    while (input >> v)
        data.solid_tiles.insert(v);

    input.close();
    return data;
}

void FileReader::SaveMapFile(const std::string& map_filepath, const MapData& map_data) const
{
    std::ofstream map_file("../assets/maps/"+map_filepath); // Create a function in file
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
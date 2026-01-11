#include "file.hpp"

WorldData FileReader::ReadWorldFile(const std::string& world_filepath) const
{
    std::ifstream input;
    input.open(world_filepath);
    WorldData data;
    
    int v;
    input >> v; data.start_map = v;
    input >> v; data.size.x = v;
    input >> v; data.size.y = v;
    
    std::string s;
    data.maps.reserve(data.size.x*data.size.y);
    while (input >> s)
        data.maps.push_back(s);

    input.close();
    return data;
}

void FileReader::ReadHeaderMapFile(std::ifstream& input, MapData& data) const
{
    int v;
    input >> v; data.layer_count = v;
    input >> v; data.size.x = v;
    input >> v; data.size.y = v;
    input >> v; data.spawn_position.x = v;
    input >> v; data.spawn_position.y = v;
    std::string s;
    while (input >> s && s != MAP_HEADER_END)
        data.tilesets.push_back(s);
}

MapData FileReader::GetMapFromFile(const std::string& path, Camera& camera, TextureController& texture_controller,
    Tileset& tileset, const bool layer_culling) const
{
    std::ifstream input;
    const std::string map_filepath = "../assets/maps/" + path; // Create a function in File
    input.open(map_filepath);
    MapData data;

    ReadHeaderMapFile(input, data);
    Tile t;
    const size_t layer_size = data.size.x*data.size.y;
    for (unsigned int i=0 ; i < data.layer_count ; i++){
        TileLayer current_layer(data.size, camera, texture_controller, tileset); // Currently, all layers have the same size
        for (size_t c=0 ; c<layer_size ; c++){
            input >> t; current_layer.AddTile(t);
        }
        current_layer.SetShouldCulling(layer_culling);
        data.map.push_back(current_layer);
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

    input >> data.size.x;
    input >> data.size.y;
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
    const int width = map_data.size.x;
    const int height = map_data.size.y;
    const int layer_count = map_data.layer_count; 
    MapPosition spawn = MapPosition{-1, -1}; // Later, make something to select in editor the spawning tile
    // Need to check if a file with the given name already exist

    // Header
    map_file << layer_count << " " << width << " " << height << " " << spawn.x << " " << spawn.y << "\n";
    for (const TextureKey& k : map_data.tilesets){ // Tileset filepath must be write in order
        map_file << k << "\n"; // TilesetKey need definition for operator<<
    }
    map_file << MAP_HEADER_END << "\n";
    
    // Map layers
    for (unsigned int layer=0 ; layer < layer_count ; layer++){
        const std::vector<Tile> layer_tiles = map_data.map[layer].GetTiles();
        for (size_t j = 0 ; j < height ; j++){
            for (size_t i = 0 ; i < width ; i++){
                map_file << layer_tiles[j*width+i] << " ";
            }
            map_file << "\n";
        }
        map_file << "\n";
    }
    map_file.close();
}
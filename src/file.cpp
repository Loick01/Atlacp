#include "file.hpp"

WorldData FileReader::ReadWorldFile(const std::string& worldFilepath) const
{
    std::ifstream input;
    input.open(worldFilepath);
    WorldData data;
    
    int v;
    input >> v; data.startMap = v;
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
    input >> v; data.layerCount = v;
    input >> v; data.size.x = v;
    input >> v; data.size.y = v;
    input >> v; data.spawnPosition.x = v;
    input >> v; data.spawnPosition.y = v;
    std::string s;
    while (input >> s && s != MAP_HEADER_END)
        data.tilesets.push_back(s);
}

MapData FileReader::GetMapFromFile(const std::string& path, Camera& camera, TextureController& textureController, 
    Tileset& tileset) const
{
    std::ifstream input;
    const std::string mapFilepath = "../assets/maps/" + path; // Create a function in File
    input.open(mapFilepath);
    MapData data;

    ReadHeaderMapFile(input, data);
    Tile t;
    const size_t layerSize = data.size.x*data.size.y;
    for (unsigned int i = 0 ; i < data.layerCount ; i++){
        TileLayer currentLayer(data.size, camera, textureController, tileset); // Currently, all layers have the same size
        for (size_t c = 0 ; c < layerSize ; c++)
            input >> t; currentLayer.AddTile(t);
        data.map.push_back(currentLayer);
    }
    input.close();
    return data;
}

AnimationData FileReader::GetAnimationFromFile(const std::string& path) const
{
    std::ifstream input;
    input.open(path);
    AnimationData data;

    input >> data.spriteSize.x;
    input >> data.spriteSize.y;
    input >> data.step;
    input >> data.frameDuration;

    int v1, v2;
    data.sprites.reserve(data.step*4);
    while (input >> v1 && input >> v2) // Use a for loop with data.step instead ?
        data.sprites.push_back(Vec2{v1, v2}*data.spriteSize);

    input.close();
    return data;
}

TilesetData FileReader::GetTilesetFromFile(const std::string& path) const
{
    std::ifstream input;
    const std::string tilesetFilepath = "../assets/tilesets/" + path; // Create a function in File
    input.open(tilesetFilepath);
    TilesetData data;

    input >> data.size.x;
    input >> data.size.y;
    input >> data.tileSize;

    int v;
    while (input >> v)
        data.solidTiles.insert(v);

    input.close();
    return data;
}

void FileReader::SaveMapFile(const std::string& mapFilepath, const MapData& mapData) const
{
    std::ofstream mapFile("../assets/maps/"+mapFilepath); // Create a function in file
    const int width = mapData.size.x;
    const int height = mapData.size.y;
    const int layerCount = mapData.layerCount; 
    MapPosition spawn = MapPosition{-1, -1}; // Later, make something to select in editor the spawning tile
    // Need to check if a file with the given name already exist

    // Header
    mapFile << layerCount << " " << width << " " << height << " " << spawn.x << " " << spawn.y << "\n";
    for (const TextureKey& k : mapData.tilesets){ // Tileset filepath must be write in order
        mapFile << k << "\n"; // TilesetKey need definition for operator<<
    }
    mapFile << MAP_HEADER_END << "\n";
    
    // Map layers
    for (unsigned int layer=0 ; layer < layerCount ; layer++){
        const std::vector<Tile> layer_tiles = mapData.map[layer].GetTiles();
        for (size_t j = 0 ; j < height ; j++){
            for (size_t i = 0 ; i < width ; i++){
                mapFile << layer_tiles[j*width+i] << " ";
            }
            mapFile << "\n";
        }
        mapFile << "\n";
    }
    mapFile.close();
}
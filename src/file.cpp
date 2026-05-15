#include "file.hpp"

std::vector<DataNPC> FileReader::ReadDataNPCs(const std::string& npcsFilepath, const unsigned int mapIndex) const
{
    std::ifstream input;
    input.open(npcsFilepath);
    std::vector<DataNPC> npcsData;
    unsigned int currentIndex = 0;
    std::string s;
    while(currentIndex < mapIndex && input >> s) { // Skip to the data associated with the chosen map
        if (s == FILE_DELIMITER) currentIndex++;
    }
    if (currentIndex != mapIndex) throw std::runtime_error("NPC file is invalid");

    while (input >> s && s != FILE_DELIMITER) {
        DataNPC data;
        // No verification yet on what is read 
        data.sprite = s;
        input >> data.position.x; input >> data.position.y;
        input >> data.walkSpeed;
        input >> data.runSpeed;
        npcsData.push_back(data);
    }
    return npcsData;
}

WorldData FileReader::ReadWorldFile(const std::string& worldFilepath) const
{
    std::ifstream input;
    input.open(worldFilepath);
    WorldData data;
    
    input >> data.startMap;
    input >> data.size.x;
    input >> data.size.y;
    
    std::string s;
    data.maps.reserve(data.size.x*data.size.y);
    while (input >> s)
        data.maps.push_back(s);

    input.close();
    return data;
}

void FileReader::ReadHeaderMapFile(std::ifstream& input, MapData& data) const
{
    input >> data.layerCount;
    input >> data.size.x;
    input >> data.size.y;
    input >> data.spawnPosition.x;
    input >> data.spawnPosition.y;
    std::string s;
    while (input >> s && s != FILE_DELIMITER)
        data.tilesets.push_back(s);
}

MapData FileReader::GetMapFromFile(const std::string& path, Camera& camera, TextureController& textureController, 
    Tileset& tileset) const
{
    std::ifstream input;
    const std::string mapFilepath = "../data/maps/" + path; // Create a function in File
    input.open(mapFilepath);
    MapData data;

    ReadHeaderMapFile(input, data);
    Tile t;
    const size_t layerSize = data.size.x*data.size.y;
    // WARNING : Use push_back on data.map reallocate its internal storage, 
    // causing existing elements to be copied/moved and their destructors to be called.
    // That's why reserve() MUST be called if I want to avoid unexpected calls of ~TileLayer() 
    data.map.reserve(data.layerCount);
    for (unsigned int i = 0 ; i < data.layerCount ; i++){
        TileLayer currentLayer(data.size, camera, textureController, tileset); // Currently, all layers have the same size
        for (size_t c = 0 ; c < layerSize ; c++){
            input >> t; 
            currentLayer.AddTile(t);
        }
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

Axis FileReader::ReadAxis(const std::string& s) const
{
    if (s == "width") return Axis::Width; // Instead of if/else, I could use a static map<string,Axis> ?
    else if (s == "height") return Axis::Height;
    else throw std::runtime_error("Unknown value read as Axis");
}

Anchor FileReader::ReadAnchor(const std::string& s) const
{
    if (s == "left_in") return Anchor::LeftIn; // Instead of if/else, I could use a static map<string,Anchor> ?
    else if (s == "left_out") return Anchor::LeftOut;
    else if (s == "right_in") return Anchor::RightIn;
    else if (s == "right_out") return Anchor::RightOut;
    else if (s == "center") return Anchor::Center;
    else if (s == "top_in") return Anchor::TopIn;
    else if (s == "top_out") return Anchor::TopOut;
    else if (s == "bottom_in") return Anchor::BottomIn;
    else if (s == "bottom_out") return Anchor::BottomOut;
    else throw std::runtime_error("Unknown value read as Anchor");
}

std::vector<DataUi> FileReader::ReadUiFile(const std::string& uiFilepath) const
{
    std::ifstream input;
    input.open(uiFilepath);
    std::vector<DataUi> uisData;
    
    std::string s;
    while (input >> s && s != FILE_DELIMITER) {
        DataUi data;
        // No verification yet on what is read 
        data.parentKey = s;
        input >> data.key; 
        input >> data.path;
        input >> data.type; // Verification on type will be in UiController
        
        // Read data for the UiParams
        while (input >> s && s != FILE_DELIMITER) {
            if (s == "scale") {
                input >> data.params.scale;
            } else if (s == "scaleAxis") {
                input >> s;
                data.params.scaleAxis = ReadAxis(s);
            } else if (s == "xAnchor") {
                input >> s;
                data.params.xAnchor = ReadAnchor(s);
            } else if (s == "yAnchor") {
                input >> s;
                data.params.yAnchor = ReadAnchor(s);
            } else if (s == "xPadding") {
                input >> data.params.xPadding; // TODO
            } else if (s == "yPadding") {
                input >> data.params.yPadding; // TODO
            } else  
                throw std::runtime_error("UiParams has no member with this name");
        }

        uisData.push_back(data);
    }
    return uisData;
}

void FileReader::SaveMapFile(const std::string& mapFilepath, const MapData& mapData) const
{
    std::ofstream mapFile("../data/maps/"+mapFilepath); // Create a function in file
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
    mapFile << FILE_DELIMITER << "\n";
    
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
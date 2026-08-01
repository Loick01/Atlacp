#include "core/file.hpp"

#include <unordered_map>

#include "image/texture.hpp"
#include "core/camera.hpp"
#include "tile/layer.hpp"
#include "tile/tileset.hpp"

std::ifstream FileReader::OpenFile(const std::string& filepath)
{
    std::ifstream input;
    input.open(filepath);
    if (!input) throw std::runtime_error("Can't open this file : " + filepath);
    return input;
}

std::string FileReader::ReadString(std::ifstream& input) const 
{
    std::string s;
    input >> s;
    std::replace(s.begin(), s.end(), '_', ' ');
    return s;
}

std::unordered_map<unsigned int, MoveDefinition> FileReader::ReadMoveFile(const std::string& moveFilepath) const
{
    std::ifstream input = OpenFile(moveFilepath);
    std::unordered_map<unsigned int, MoveDefinition> moves;
    std::string s;
    while (input >> s) {
        MoveDefinition m;
        const unsigned int moveId = std::stoi(s); // Should catch invalid_argument ?
        
        m.name = ReadString(input);
        m.commandType = ReadCommandType(input);
        m.moveType = ReadMoveType(input);
        input >> m.value;
        input >> m.sfxPath;
        input >> m.animationPath;
        moves[moveId] = m;
    }
    return moves;
}

std::vector<DataBattleActor> FileReader::ReadBattleFile(const std::string& battleFilepath) const
{
    std::ifstream input = OpenFile(battleFilepath);
    std::vector<DataBattleActor> actorsData;
    std::string s;
    
    while (input >> s && s != FILE_DELIMITER) {
        DataBattleActor data;
        data.isAiActor = true;
        if (s == "ally") data.team = Team::Ally;
        else if (s == "opponent") data.team = Team::Opponent;
        
        // ?
        input >> s;
        if (s == "ai") data.isAiActor = true;
        else if (s == "player") data.isAiActor = false;
        else throw std::runtime_error("Error FileReader::ReadBattleFile : Must be \"ai\" or \"player\"");
        
        data.name = ReadString(input);
        input >> data.health;
        input >> data.turnSpeed;
        input >> data.spritePath;
        while (input >> s && s != FILE_DELIMITER)
            data.moveIds.push_back(std::stoi(s)); // Should catch invalid_argument ? 
        
        actorsData.push_back(data);
    }
    
    return actorsData;
}

std::vector<DataNPC> FileReader::ReadNPCsFile(const std::string& npcsFilepath, const unsigned int mapIndex) const
{
    std::ifstream input = OpenFile(npcsFilepath);
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
        // NPC always spawn with random behaviour
        npcsData.push_back(data);
    }
    return npcsData;
}

std::vector<DataUi> FileReader::ReadUiFile(const std::string& uiFilepath) const
{
    std::ifstream input = OpenFile(uiFilepath);
    std::vector<DataUi> uisData;
    
    std::string s;
    while (input >> s && s != FILE_DELIMITER) {
        if (s == "load") {
            input >> s;
            std::vector<DataUi> load = ReadUiFile(s);
            uisData.insert(std::end(uisData), std::begin(load), std::end(load));
            input >> s; // After the load line, the next line must be FILE_DELIMITER
            continue;
        }
        DataUi data;
        // No verification yet on what is read 
        data.parentKey = s;
        input >> data.key; 
        input >> data.type; // Verification on type will be in UiController
        if (data.type == "uielement" || data.type == "animatedelement") // Not for "textelement"
            input >> data.imagePath;
        
        // Read optional data 
        while (input >> s && s != FILE_DELIMITER) {
            if (s == "scale") {
                input >> data.scale.srcElement;
                data.scale.axis = ReadAxis(input);
                input >> data.scale.amount;
            } else if (s == "scaleAxis") {
                data.dstScaleAxis = ReadAxis(input);
            } else if (s == "xAnchor") {
                data.xAnchor = ReadAnchor(input);
            } else if (s == "yAnchor") {
                data.yAnchor = ReadAnchor(input);
            } else if (s == "xPadding") {
                input >> data.xPadding.srcElement;
                data.xPadding.axis = ReadAxis(input);
                input >> data.xPadding.amount;
            } else if (s == "yPadding") {
                input >> data.yPadding.srcElement;
                data.yPadding.axis = ReadAxis(input);
                input >> data.yPadding.amount;
            } else if (s == "text") {
                std::getline(input >> std::ws, data.text); // std::ws discards leading whitespace from input stream (should use ReadString ?)
            } else if (s == "textsize") {
                data.fontSize = ReadFontSize(input);
            } else  
                throw std::runtime_error("UiParams has no member with this name " + s);
        }
        uisData.push_back(data);
    }
    return uisData;
}

WorldData FileReader::ReadWorldFile(const std::string& worldFilepath) const
{
    std::ifstream input = OpenFile(worldFilepath);
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

MapData FileReader::ReadMapFile(const std::string& path, Camera& camera, TextureController& textureController, 
    Tileset& tileset) const
{
    std::ifstream input = OpenFile("../data/maps/" + path); // Create a function to get full path in FilepathManager ?
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

AnimationData FileReader::ReadAnimationFile(const std::string& path) const
{
    std::ifstream input = OpenFile(path);
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

TilesetData FileReader::ReadTilesetFile(const std::string& path) const
{
    std::ifstream input = OpenFile("../assets/tilesets/" + path);
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

Axis FileReader::ReadAxis(std::ifstream& input) const
{
    static const std::unordered_map<std::string, Axis> axis = {
        {"width", Axis::Width},
        {"height", Axis::Height}
    };

    std::string s;
    input >> s;
    std::unordered_map<std::string, Axis>::const_iterator it = axis.find(s);
    if (it != axis.end())
        return it->second;

    throw std::runtime_error("Unknown value read as Axis");
}

Anchor FileReader::ReadAnchor(std::ifstream& input) const
{
    static const std::unordered_map<std::string, Anchor> anchors = {
        {"left_in", Anchor::LeftIn},
        {"left_out", Anchor::LeftOut},
        {"right_in", Anchor::RightIn},
        {"right_out", Anchor::RightOut},
        {"center", Anchor::Center},
        {"top_in", Anchor::TopIn},
        {"top_out", Anchor::TopOut},
        {"bottom_in", Anchor::BottomIn},
        {"bottom_out", Anchor::BottomOut}
    };

    std::string s;
    input >> s;
    std::unordered_map<std::string, Anchor>::const_iterator it = anchors.find(s);
    if (it != anchors.end())
        return it->second;

    throw std::runtime_error("Unknown value read as Anchor");
}

CommandType FileReader::ReadCommandType(std::ifstream& input) const
{
    static const std::unordered_map<std::string, CommandType> commands = {
        {"attack", CommandType::Attack},
        {"heal", CommandType::Heal}
    };

    std::string s;
    input >> s;
    std::unordered_map<std::string, CommandType>::const_iterator it = commands.find(s);
    if (it != commands.end())
        return it->second;

    throw std::runtime_error("Unknown value read as CommandType");
}

MoveType FileReader::ReadMoveType(std::ifstream& input) const
{
    static const std::unordered_map<std::string, MoveType> moves = {
        {"physical", MoveType::Physical},
        {"magic", MoveType::Magic}
    };

    std::string s;
    input >> s;
    std::unordered_map<std::string, MoveType>::const_iterator it = moves.find(s);
    if (it != moves.end())
        return it->second;

    throw std::runtime_error("Unknown value read as MoveType");
}

MapBehaviour FileReader::ReadMapBehaviour(std::ifstream& input) const
{
    static const std::unordered_map<std::string, MapBehaviour> behaviours = {
        {"random", MapBehaviour::Random},
        {"follow", MapBehaviour::Follow},
        {"goto", MapBehaviour::GoTo}
    };

    std::string s;
    input >> s;
    std::unordered_map<std::string, MapBehaviour>::const_iterator it = behaviours.find(s);
    if (it != behaviours.end())
        return it->second;

    throw std::runtime_error("Unknown value read as MapBehaviour");
}

FontSize FileReader::ReadFontSize(std::ifstream& input) const
{
    static const std::unordered_map<std::string, FontSize> fontSizes = {
        {"small", FontSize::Small}
    };

    std::string s;
    input >> s;
    std::unordered_map<std::string, FontSize>::const_iterator it = fontSizes.find(s);
    if (it != fontSizes.end())
        return it->second;

    throw std::runtime_error("Unknown value read as FontSize");
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
#include "core/file/file.hpp"

#include <unordered_map>

#include "image/texture.hpp" // ?
#include "core/camera.hpp" // ?
#include "tile/layer.hpp"
#include "tile/tileset.hpp" // ?

namespace DataDirectory { // Should be in core/path.hpp ?
    // Should be constexpr std::string_view ?
    const std::string Animation = "../data/animation/";
    const std::string Battle = "../data/battle/battles/";
    const std::string Map = "../data/maps/";
    const std::string Move = "../data/battle/moves/";
    const std::string NPC = "../data/npcs/";
    const std::string Tileset = "../data/tileset/";
    const std::string UiFile = "../data/ui/file/";
    const std::string UiTemplate = "../data/ui/template/";
    const std::string World = "../data/worlds/";

    const std::string BattleMoveDirectory = "battle_move/";
}

std::unordered_map<unsigned int, MoveDefinition> FileReader::ReadMoveFile(const std::string& moveFilepath) const
{
    std::ifstream input = OpenFile(DataDirectory::Move + moveFilepath);
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
        input >> s;
        m.animationPath = DataDirectory::BattleMoveDirectory+s;
        moves[moveId] = m;
    }
    return moves;
}

std::vector<DataBattleActor> FileReader::ReadBattleFile(const std::string& battleFilepath) const
{
    std::ifstream input = OpenFile(DataDirectory::Battle + battleFilepath);
    std::vector<DataBattleActor> actorsData;
    std::string s;
    
    while (input >> s && s != SECTION_DELIMITER) {
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
        while (input >> s && s != SECTION_DELIMITER)
            data.moveIds.push_back(std::stoi(s)); // Should catch invalid_argument ? 
        
        actorsData.push_back(data);
    }
    
    return actorsData;
}

std::vector<DataNPC> FileReader::ReadNPCsFile(const std::string& npcsFilepath, const unsigned int mapIndex) const
{
    std::ifstream input = OpenFile(DataDirectory::NPC + npcsFilepath);
    std::vector<DataNPC> npcsData;
    unsigned int currentIndex = 0;
    std::string s;
    while(currentIndex < mapIndex && input >> s) { // Skip to the data associated with the chosen map
        if (s == SECTION_DELIMITER) currentIndex++;
    }
    if (currentIndex != mapIndex) throw std::runtime_error("NPC file is invalid : " + npcsFilepath);

    while (input >> s && s != SECTION_DELIMITER) {
        DataNPC data;
        // No verification yet on what is read 
        data.sprite = s;
        input >> data.position.x; input >> data.position.y;
        input >> data.walkSpeed;
        input >> data.runSpeed;
        // NPC always spawn with random behaviour

        while (input >> s && s != MAP_ELEMENT_DELIMITER) {
            // Merge with FileReader::ReadMapElement()
            if (s == "frame_text") {
                data.orders.push_back(ReadFrameTextOrder(input));
            } else if (s == "dialog_text") {
                data.orders.push_back(ReadDialogTextOrder(input));
            } else if (s == "npc_goto") {
                data.orders.push_back(ReadNpcGoToOrder(input));
            } else {
                throw std::runtime_error("Unknow order type : " + s);
            }
        }

        npcsData.push_back(data);
    }
    return npcsData;
}

std::vector<DataUi> FileReader::ReadUiFile(const std::string& uiFilepath) const
{
    std::ifstream input;
    if (IsBaseUiFile(uiFilepath))
        input = OpenFile(DataDirectory::UiFile + uiFilepath);
    else if (IsTemplateUiFile(uiFilepath))
        input = OpenFile(DataDirectory::UiTemplate + uiFilepath);

    std::vector<DataUi> uisData;
    std::string s;
    while (input >> s && s != SECTION_DELIMITER) {
        if (s == "load") {
            input >> s;
            std::vector<DataUi> load = ReadUiFile(s);
            uisData.insert(std::end(uisData), std::begin(load), std::end(load));
            input >> s; // After the load line, the next line must be SECTION_DELIMITER
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
        while (input >> s && s != SECTION_DELIMITER) {
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
    std::ifstream input = OpenFile(DataDirectory::World + worldFilepath);
    WorldData data;
    
    input >> data.startMap;
    input >> data.size.x;
    input >> data.size.y;
    input >> data.npcsFile;
    
    std::string s;
    data.maps.reserve(data.size.x*data.size.y);
    while (input >> s)
        data.maps.push_back(s);

    input.close();
    return data;
}

DataMapElement FileReader::ReadMapElement(std::ifstream& input) const
{
    DataMapElement e;
    input >> e.position.x;
    input >> e.position.y;

    std::string s;
    while (input >> s && s != MAP_ELEMENT_DELIMITER) { // Multiple orders
        if (s == "frame_text") {
            e.orders.push_back(ReadFrameTextOrder(input));
        } else if (s == "dialog_text") {
            e.orders.push_back(ReadDialogTextOrder(input));
        } else if (s == "npc_goto") {
            e.orders.push_back(ReadNpcGoToOrder(input));
        } else {
            throw std::runtime_error("Unknow order type : " + s);
        }
    }
    return e;
}

void FileReader::ReadHeaderMapFile(std::ifstream& input, MapData& data) const
{
    input >> data.layerCount;
    input >> data.size.x;
    input >> data.size.y;
    input >> data.spawnPosition.x;
    input >> data.spawnPosition.y;
    
    std::string s;
    while (input >> s && s != SECTION_DELIMITER)
        data.tilesets.push_back(s);

    while (input >> s && s != SECTION_DELIMITER) {
        const DataMapElement e = ReadMapElement(input);
        data.elements.push_back(e);
    }
}

MapData FileReader::ReadMapFile(const std::string& mapFilepath, Camera& camera, TextureController& textureController, 
    Tileset& tileset) const
{
    std::ifstream input = OpenFile(DataDirectory::Map + mapFilepath);
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

AnimationData FileReader::ReadAnimationFile(const std::string& animationFilepath) const
{
    std::ifstream input = OpenFile(DataDirectory::Animation + animationFilepath);
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
    std::ifstream input = OpenFile(DataDirectory::Tileset + path);
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
    std::ofstream mapFile(DataDirectory::Map + mapFilepath);
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
    mapFile << SECTION_DELIMITER << "\n";
    
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
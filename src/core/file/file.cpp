#include "core/file/file.hpp"

#include <unordered_map>

#include "core/camera.hpp"
#include "core/path.hpp"
#include "image/texture.hpp"
#include "tile/layer.hpp" // MapData, TileLayer
#include "tile/tileset.hpp"

std::unordered_map<unsigned int, MoveDefinition> FileReader::ReadMoveFile(const std::string& moveFilepath) const
{
    std::optional<std::ifstream> optInput = OpenFile(std::string(DataDirectory::Move) + moveFilepath);
    if (!optInput.has_value())
        return {};
    std::ifstream& input = optInput.value();
    
    std::unordered_map<unsigned int, MoveDefinition> moves;
    std::string s;
    while (input >> s) {
        MoveDefinition m;
        const unsigned int moveId = std::stoi(s); // TODO : Should catch invalid_argument ?
        
        m.name = ReadString(input);
        m.commandType = ReadCommandType(input);
        m.moveType = ReadMoveType(input);
        input >> m.baseValue;
        input >> m.sfxPath;
        input >> s;
        m.animationPath = std::string(DataDirectory::BattleMoveDirectory)+s;
        moves[moveId] = m;
    }
    return moves;
}

std::vector<DataBattleActor> FileReader::ReadBattleFile(const std::string& battleFilepath) const
{
    // TODO : Add file extension to battle file
    
    std::optional<std::ifstream> optInput = OpenFile(std::string(DataDirectory::Battle) + battleFilepath);
    if (!optInput.has_value())
        return {};
    std::ifstream& input = optInput.value();

    std::vector<DataBattleActor> actorsData;
    std::string s;
    
    while (input >> s && s != SECTION_DELIMITER) {
        DataBattleActor data;
        data.isAiActor = true;
        if (s == "ally") data.team = Team::Ally;
        else if (s == "opponent") data.team = Team::Opponent;
        else {
            Logger::GetInstance().AddLog(Level::L4, "Error FileReader::ReadBattleFile : Must be \"ally\" or \"opponent\""); // TODO : Log
            break;
        }
        
        // TODO : ?
        input >> s;
        if (s == "ai") data.isAiActor = true;
        else if (s == "player") data.isAiActor = false;
        else {
            Logger::GetInstance().AddLog(Level::L4, "Error FileReader::ReadBattleFile : Must be \"ai\" or \"player\""); // TODO : Log
            break;
        }

        data.name = ReadString(input);
        input >> data.health;
        input >> data.turnSpeed;
        input >> data.spritePath;
        while (input >> s && s != SECTION_DELIMITER)
            data.moveIds.push_back(std::stoi(s)); // TODO : Should catch invalid_argument ? 
        
        actorsData.push_back(data);
    }
    
    return actorsData;
}

DataNPC FileReader::ReadDataNPC(std::ifstream& input, const unsigned int targetId) const
{
    DataNPC data; // NPC always spawn with random behaviour

    input >> data.id;
    input >> data.sprite; 
    input >> data.position.x; input >> data.position.y;
    input >> data.walkSpeed;
    input >> data.runSpeed;
    data.orders = ReadOrders(input);
    
    return data;
}

std::vector<DataNPC> FileReader::GetDataNPCsForMap(const std::string& npcsFilepath, const unsigned int mapIndex) const
{
    if (!CheckFileExtension(npcsFilepath, FileExtension::WorldNpcs)) {
        Logger::GetInstance().AddLog(Level::L4, "World Npc file extension is incorrect"); // TODO : Level
        return {};
    }

    std::optional<std::ifstream> optInput = OpenFile(std::string(DataDirectory::NPC) + npcsFilepath);
    if (!optInput.has_value())
        return {};
    std::ifstream& input = optInput.value();

    std::vector<DataNPC> npcsData;
    unsigned int count = 0;
    std::string s;
    
    while(count < mapIndex && input >> s) { // Skip to the data associated with the chosen map
        if (s == SECTION_DELIMITER) count++;
    }
    if (count != mapIndex) {
        Logger::GetInstance().AddLog(Level::L4, "NPC file is invalid: " + npcsFilepath); // TODO : Log
        return {};
    }

    input >> count;
    for (unsigned int i = 0 ; i < count ; i++) {
        const DataNPC data = ReadDataNPC(input);
        npcsData.push_back(data);
    }
    
    return npcsData;
}

std::optional<DataNPC> FileReader::GetDataNPCFromId(const std::string& npcFilepath, const unsigned int idEntity) const
{
    if (!CheckFileExtension(npcFilepath, FileExtension::Npcs)) {
        Logger::GetInstance().AddLog(Level::L4, "Npcs file extension is incorrect"); // TODO : Level
        return std::nullopt;
    }

    std::optional<std::ifstream> optInput = OpenFile(std::string(DataDirectory::NPC) + npcFilepath);
    if (!optInput.has_value())
        std::nullopt;
    std::ifstream& input = optInput.value();

    std::string s;

    while (input >> s) {
        const DataNPC data = ReadDataNPC(input, idEntity);
        if (data.id == idEntity)
            return data;
    }
    Logger::GetInstance().AddLog(Level::L4, "No NPC was found with in file " + npcFilepath + " with id = " + std::to_string(idEntity)); // TODO : Log
    return std::nullopt;
}

std::vector<DataUi> FileReader::ReadUiFile(const std::string& uiFilepath) const
{
    std::optional<std::ifstream> optInput;
    if (CheckFileExtension(uiFilepath, FileExtension::UiFile))
        optInput = OpenFile(std::string(DataDirectory::UiFile) + uiFilepath);
    else if (CheckFileExtension(uiFilepath, FileExtension::UiTemplate))
        optInput = OpenFile(std::string(DataDirectory::UiTemplate) + uiFilepath);

    if (!optInput.has_value())
        return {};
    std::ifstream& input = optInput.value();

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
                std::getline(input >> std::ws, data.text); // std::ws discards leading whitespace from input stream // TODO : Should use ReadString ?
            } else if (s == "textsize") {
                data.fontSize = ReadFontSize(input);
            } else {
                Logger::GetInstance().AddLog(Level::L4, "UiParams has no member with this name: " + s); // TODO : Log
                return uisData; // TODO : ?
            }
        }
        uisData.push_back(data);
    }
    return uisData;
}

std::optional<WorldData> FileReader::ReadWorldFile(const std::string& worldFilepath) const
{
    WorldData data;
    std::optional<std::ifstream> optInput = OpenFile(std::string(DataDirectory::World) + worldFilepath);
    if (!optInput.has_value())
        return std::nullopt;
    std::ifstream& input = optInput.value();
    
    input >> data.startMap;
    input >> data.size.x;
    input >> data.size.y;
    input >> data.mapDirectoryPath;
    input >> data.spritePlayerPath;
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
    e.orders = ReadOrders(input);
    
    return e;
}

void FileReader::ReadHeaderMapFile(std::ifstream& input, MapData& data) const
{
    std::string s;
    unsigned int count = 0;
    
    input >> data.layerCount;
    input >> data.layerSplitIndex;
    input >> data.size.x;
    input >> data.size.y;
    input >> data.spawnPosition.x;
    input >> data.spawnPosition.y;

    input >> s; // Skip "tileset"
    input >> count;
    input >> data.tilesetDirectoryPath;
    for (unsigned int i = 0 ; i < count ; i++) {
        input >> s;
        data.tilesets.push_back(s);
    }

    input >> s; // Skip "element"
    input >> count;
    for (unsigned int i = 0 ; i < count ; i++) {
        const DataMapElement e = ReadMapElement(input);
        data.elements.push_back(e);
    }

    input >> s; // Skip "trigger"
    input >> count;
    for (unsigned int i = 0 ; i < count ; i++) {
        const DataMapElement e = ReadMapElement(input);
        data.triggers.push_back(e);
    }
}

MapData FileReader::ReadMapFile(const std::string& mapFilepath, Camera& camera, TextureController& textureController, 
    Tileset& tileset) const
{
    std::optional<std::ifstream> optInput = OpenFile(std::string(DataDirectory::Map) + mapFilepath);
    // TODO : has_value()
    std::ifstream& input = optInput.value();

    MapData data;

    ReadHeaderMapFile(input, data);
    Tile t;
    const size_t layerSize = data.size.x*data.size.y;
    // WARNING : Use push_back on data.layers reallocate its internal storage, 
    // causing existing elements to be copied/moved and their destructors to be called.
    // That's why reserve() MUST be called if I want to avoid unexpected calls of ~TileLayer() 
    data.layers.reserve(data.layerCount);
    for (unsigned int i = 0 ; i < data.layerCount ; i++){
        TileLayer currentLayer(data.size, camera, textureController, tileset); // Currently, all layers have the same size
        for (size_t c = 0 ; c < layerSize ; c++){
            input >> t; 
            currentLayer.AddTile(t);
        }
        data.layers.push_back(currentLayer);
    }
    input.close();
    return data;
}

SpriteAnimationData FileReader::ReadAnimationFile(const std::string& animationFilepath) const
{
    std::optional<std::ifstream> optInput = OpenFile(std::string(DataDirectory::SpriteAnimation) + animationFilepath);
    // TODO : has_value()
    std::ifstream& input = optInput.value();

    SpriteAnimationData data;

    input >> data.spriteSize.x;
    input >> data.spriteSize.y;
    input >> data.step;
    input >> data.frameDuration;

    int v1, v2;
    data.sprites.reserve(data.step*4);
    while (input >> v1 && input >> v2) // TODO : Use a for loop with data.step instead ?
        data.sprites.push_back(Vec2{v1, v2}*data.spriteSize);

    input.close();
    return data;
}

TilesetData FileReader::ReadTilesetFile(const std::string& path) const
{
    std::optional<std::ifstream> optInput = OpenFile(std::string(DataDirectory::Tileset) + path);
    // TODO : has_value()
    std::ifstream& input = optInput.value();

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
    std::ofstream output(std::string(DataDirectory::Map) + mapFilepath);
    const int width = mapData.size.x;
    const int height = mapData.size.y;
    const int layerCount = mapData.layerCount; 

    output << layerCount << " " << mapData.layerSplitIndex << " " << width << " " << height << " " << mapData.spawnPosition.x << " " << mapData.spawnPosition.y << "\n\n";
    
    // Tilesets
    output << "tileset " << mapData.tilesets.size() << " ";
    output << mapData.tilesetDirectoryPath << "\n";
    for (const TextureKey& k : mapData.tilesets) { // Tileset filepath must be write in order
        output << k << "\n"; // TilesetKey need definition for operator<<
    }
    
    // MapElement (with Orders)
    output << "\nelement " << mapData.elements.size() << "\n\n";
    for (const DataMapElement& me : mapData.elements) {
        output << me.position.x << " " << me.position.y << " " << me.orders.size() << "\n";
        for (const Order& o : me.orders)
            output << GetStringDescription(o) << "\n";

        output << "\n";
    }

    // Trigger (with Orders)
    output << "trigger " << mapData.triggers.size() << "\n\n";
    for (const DataMapElement& me : mapData.triggers) {
        output << me.position.x << " " << me.position.y << " " << me.orders.size() << "\n";
        for (const Order& o : me.orders)
            output << GetStringDescription(o) << "\n";

        output << "\n";
    }

    // Map layers
    for (unsigned int l=0 ; l < layerCount ; l++){
        const std::vector<Tile> tiles = mapData.layers[l].GetTiles();
        for (size_t j = 0 ; j < height ; j++){
            for (size_t i = 0 ; i < width ; i++){
                output << tiles[j*width+i] << " ";
            }
            output << "\n";
        }
        output << "\n";
    }
    output.close();
}
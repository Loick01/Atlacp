#pragma once

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "animation/animation_types.hpp" // AnimationData
#include "battle/battle_types.hpp" // DataBattleActor
#include "map/interaction/order.hpp" // Order
#include "tile/tile_types.hpp" // WorldData, TilesetData
#include "ui/ui_types.hpp" // DataUi

#define SECTION_DELIMITER "###" // define ? Rename
#define MAP_ELEMENT_DELIMITER "##" // define ? Rename
#define ORDER_DELIMITER "#" // define ? Rename
#define TEXT_DELIMITER ';' // define ? Rename

class Camera;
class TextureController;
class Tileset;

struct MapData;

struct DataNPC // Rename ? Should not be here ?
{
    std::vector<Order> orders;
    MapPosition position;
    std::string sprite;
    float walkSpeed;
    float runSpeed;
    unsigned int id;
};

struct DataMapElement // Rename ? // Should not be here ?
{
    std::vector<Order> orders;
    MapPosition position;
};

class FileReader
{
    private:
        static std::ifstream OpenFile(const std::string& filepath);
        std::string ReadString(std::ifstream& input) const;

        // Should not be here ?
        Axis ReadAxis(std::ifstream& input) const;
        Anchor ReadAnchor(std::ifstream& input) const;
        CommandType ReadCommandType(std::ifstream& input) const;
        MoveType ReadMoveType(std::ifstream& input) const; // Rename ?
        MapBehaviour ReadMapBehaviour(std::ifstream& input) const; // Will be used for cinematic files
        FontSize ReadFontSize(std::ifstream& input) const;

        FrameTextOrder ReadFrameTextOrder(std::ifstream& input) const;
        DialogTextOrder ReadDialogTextOrder(std::ifstream& input) const;
        NpcGoToOrder ReadNpcGoToOrder(std::ifstream& input) const;
        
        DataMapElement ReadMapElement(std::ifstream& input) const;
        void ReadHeaderMapFile(std::ifstream& input, MapData& m) const;

    public:
        FileReader() = default;

        std::unordered_map<unsigned int, MoveDefinition> ReadMoveFile(const std::string& moveFilepath) const;
        std::vector<DataBattleActor> ReadBattleFile(const std::string& battleFilepath) const;
        std::vector<DataNPC> ReadNPCsFile(const std::string& npcsFilepath, const unsigned int mapIndex) const;
        std::vector<DataUi> ReadUiFile(const std::string& uiFilepath) const; // Rename ? (because UiFile != UiTemplate)
        
        WorldData ReadWorldFile(const std::string& worldFilepath) const;
        MapData ReadMapFile(const std::string& mapFilepath, Camera& camera, TextureController& textureController,
            Tileset& tileset) const;
        TilesetData ReadTilesetFile(const std::string& path) const;
        AnimationData ReadAnimationFile(const std::string& animationFilepath) const;
        
        std::string GetFileExtension(const std::string& filepath) const; // static ?
        bool IsBaseUiFile(const std::string& filepath) const;
        bool IsTemplateUiFile(const std::string& filepath) const;
        
        void SaveMapFile(const std::string& mapFilepath, const MapData& mapData) const;
};
#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "animation/animation_types.hpp" // AnimationData
#include "battle/battle_types.hpp" // DataBattleActor
#include "map/map_types.hpp" // DataNPC
#include "tile/tile_types.hpp" // WorldData, TilesetData
#include "ui/ui_types.hpp" // DataUi

#define FILE_DELIMITER "###" // Will be removed ?

class Camera;
class TextureController;
class Tileset;

struct MapData;

class FileReader
{
    private:
        static std::ifstream OpenFile(const std::string& filepath);

        Axis ReadAxis(const std::string& s) const;
        Anchor ReadAnchor(const std::string& s) const;
        CommandType ReadCommandType(const std::string& s) const;
        MoveType ReadMoveType(const std::string& s) const;
        void ReadHeaderMapFile(std::ifstream& input, MapData& m) const;

    public:
        FileReader() = default;

        std::vector<DataBattleActor> ReadBattleFile(const std::string& battleFilepath) const; // Rename
        std::vector<MoveDefinition> ReadMoveFile(const std::string& moveFilepath) const; // Rename + I will use a function that return a MoveDefinition associated to an ID 
        std::vector<DataNPC> ReadNPCsFile(const std::string& npcsFilepath, const unsigned int mapIndex) const; // Rename
        std::vector<DataUi> ReadUiFile(const std::string& uiFilepath) const; // Rename
        
        WorldData ReadWorldFile(const std::string& worldFilepath) const; // Rename
        MapData GetMapFromFile(const std::string& path, Camera& camera, TextureController& textureController, // Rename
            Tileset& tileset) const;
        TilesetData GetTilesetFromFile(const std::string& path) const; // Rename
        AnimationData GetAnimationFromFile(const std::string& path) const; // Rename
        
        void SaveMapFile(const std::string& mapFilepath, const MapData& mapData) const;
};
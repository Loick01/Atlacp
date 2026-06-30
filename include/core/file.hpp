#pragma once

#include <fstream>
#include <string>
#include <unordered_map>
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
        std::string ReadString(std::ifstream& input) const;

        // Should not be here ?
        Axis ReadAxis(std::ifstream& input) const;
        Anchor ReadAnchor(std::ifstream& input) const;
        CommandType ReadCommandType(std::ifstream& input) const;
        MoveType ReadMoveType(std::ifstream& input) const;
        MapBehaviour ReadMapBehaviour(std::ifstream& input) const; // Will be used for cinematic files
        FontSize ReadFontSize(std::ifstream& input) const;

        void ReadHeaderMapFile(std::ifstream& input, MapData& m) const;

    public:
        FileReader() = default;

        std::unordered_map<unsigned int, MoveDefinition> ReadMoveFile(const std::string& moveFilepath) const; // Rename
        std::vector<DataBattleActor> ReadBattleFile(const std::string& battleFilepath) const; // Rename
        std::vector<DataNPC> ReadNPCsFile(const std::string& npcsFilepath, const unsigned int mapIndex) const; // Rename
        std::vector<DataUi> ReadUiFile(const std::string& uiFilepath) const; // Rename
        
        WorldData ReadWorldFile(const std::string& worldFilepath) const; // Rename
        MapData GetMapFromFile(const std::string& path, Camera& camera, TextureController& textureController, // Rename
            Tileset& tileset) const;
        TilesetData GetTilesetFromFile(const std::string& path) const; // Rename
        AnimationData GetAnimationFromFile(const std::string& path) const; // Rename
        
        void SaveMapFile(const std::string& mapFilepath, const MapData& mapData) const;
};
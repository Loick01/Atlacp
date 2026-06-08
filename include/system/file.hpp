#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "battle/battle_types.hpp"
#include "system/type.hpp"
#include "ui/ui_types.hpp"

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
        void ReadHeaderMapFile(std::ifstream& input, MapData& m) const;

    public:
        FileReader() = default;

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
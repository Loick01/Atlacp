#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "layer.hpp"
#include "type.hpp" // Try to remove ?
#include "ui.hpp" // I need UiParams

#define FILE_DELIMITER "###" // Will be removed ?

struct UiResult // Rename ? Should not be here ?
{
    std::string parentKey; // If "root", the current UiElement is a subroot, thus it will be created with BuildSubRoot instead of BuildChild
    std::string key;
    std::string path; // image path if UiElement, font filepath if TextArea
    UiType type; // UiElement or TextArea
    // UiParams params;

    UiResult() {
        type = UiType::Invalid; // Temp : Invalid will mean that the end of file has been reached, so the UiResult should not be used to build a UiElement
    }
};

class FileReader
{
    private:
        void ReadHeaderMapFile(std::ifstream& input, MapData& m) const;

        std::ifstream m_input; // Currently only used for ReadUiFile

    public:
        FileReader() = default;

        std::vector<DataNPC> ReadDataNPCs(const std::string& npcsFilepath, const unsigned int mapIndex) const; // Rename
        WorldData ReadWorldFile(const std::string& worldFilepath) const; // Rename
        MapData GetMapFromFile(const std::string& path, Camera& camera, TextureController& textureController, // Rename
            Tileset& tileset) const;
        TilesetData GetTilesetFromFile(const std::string& path) const; // Rename
        AnimationData GetAnimationFromFile(const std::string& path) const; // Rename

        void OpenFile(const std::string& filepath); // Currently only used for ReadUiFile
        UiResult ReadUiElement(); // Rename
        
        void SaveMapFile(const std::string& mapFilepath, const MapData& mapData) const;
};
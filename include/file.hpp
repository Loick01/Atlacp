#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "layer.hpp"
#include "type.hpp" // Try to remove ?
#include "ui_element.hpp" // I need UiParams

#define FILE_DELIMITER "###" // Will be removed ?

struct PartialSize
{
    ElementKey srcElement;
    Axis axis;
    float amount;

    PartialSize() {
        srcElement = "undefined_element"; // In UiController, I use srcElement to know if there is a padding
    }
};

struct DataUi // Rename + Should not be here ?
{
    ElementKey parentKey; // If "root", the current UiElement is a subroot, thus it will be created with BuildSubRoot instead of BuildChild
    ElementKey key;
    std::string path; // image path if UiElement, font filepath if TextArea
    std::string type; // "uielement" or "textarea"
    std::string text; // Only used for TextArea (when type = "textarea"), should not be here ?
    // Can't use UiParams
    PartialSize scale;
    Axis dstScaleAxis; // Only use for UiElement (when type = "uielement"), should not be here ?
    Anchor xAnchor;
    Anchor yAnchor;
    PartialSize xPadding;
    PartialSize yPadding;

    DataUi() {
        text = "invalid_text"; // Should not happen
    }
};

class FileReader
{
    private:
        void ReadHeaderMapFile(std::ifstream& input, MapData& m) const;
        Axis ReadAxis(const std::string& s) const;
        Anchor ReadAnchor(const std::string& s) const;

    public:
        FileReader() = default;

        std::vector<DataNPC> ReadDataNPCs(const std::string& npcsFilepath, const unsigned int mapIndex) const; // Rename
        WorldData ReadWorldFile(const std::string& worldFilepath) const; // Rename
        MapData GetMapFromFile(const std::string& path, Camera& camera, TextureController& textureController, // Rename
            Tileset& tileset) const;
        TilesetData GetTilesetFromFile(const std::string& path) const; // Rename
        AnimationData GetAnimationFromFile(const std::string& path) const; // Rename

        std::vector<DataUi> ReadUiFile(const std::string& uiFilepath) const; // Rename
        
        void SaveMapFile(const std::string& mapFilepath, const MapData& mapData) const;
};
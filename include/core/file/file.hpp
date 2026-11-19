#pragma once

#include <fstream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "animation/animation_types.hpp" // DataSpriteAnimation
#include "battle/battle_types.hpp" // DataBattleActor
#include "map/map_element_types.hpp" // DataNPC, DataMapElement
#include "map/order/order.hpp" // Order
#include "tile/tile_types.hpp" // DataWorld, DataTileset
#include "tile/tile_map_types.hpp" // DataMap
#include "ui/ui_types.hpp" // DataUiElement

#define MAJOR_DELIMITER "###"
#define MINOR_DELIMITER ';'

class Camera;
class TextureController;
class Tileset;

class FileReader
{
    private:
        // FileIO
        static std::optional<std::ifstream> OpenFile(const std::string& filepath);

        std::optional<int> GetIntFromStr(const std::string& s) const;
        std::optional<float> GetFloatFromStr(const std::string& s) const;

        std::string ReadString(std::ifstream& input) const; // TODO : Return std::optional ?
        int ReadInt(std::ifstream& input) const;
        float ReadFloat(std::ifstream& input) const;

        // FileUtils
        template <typename T>
            T ReadEnum(std::ifstream& input, const std::unordered_map<std::string, T>& values, const std::string& typeName) const;

        Axis ReadAxis(std::ifstream& input) const;
        Anchor ReadAnchor(std::ifstream& input) const;
        CommandType ReadCommandType(std::ifstream& input) const;
        MoveType ReadMoveType(std::ifstream& input) const;
        MapBehaviour ReadMapBehaviour(std::ifstream& input) const; // Will be used for cinematic files
        FontSize ReadFontSize(std::ifstream& input) const;
        Direction ReadDirection(std::ifstream& input) const;

        // FileOrder
        std::vector<Order> ReadOrders(std::ifstream& input) const;
        Order ReadFrameTextOrder(std::ifstream& input) const;
        Order ReadDialogTextOrder(std::ifstream& input) const;
        Order ReadNpcGoToOrder(std::ifstream& input) const;
        Order ReadNpcFollowOrder(std::ifstream& input) const;
        Order ReadNpcIdleOrder(std::ifstream& input) const;
        Order ReadPlayCinematicOrder(std::ifstream& input) const;
        Order ReadCameraSlideToPositionOrder(std::ifstream& input) const;
        Order ReadCameraSlideToEntityOrder(std::ifstream& input) const;
        Order ReadCameraAnchorEntityOrder(std::ifstream& input) const;
        Order ReadEntityOrientationOrder(std::ifstream& input) const;
        Order ReadEntityCreateOrder(std::ifstream& input) const;
        Order ReadEntityDeleteOrder(std::ifstream& input) const;
        Order ReadTimeDelayOrder(std::ifstream& input) const;
        Order ReadLoadMapOrder(std::ifstream& input) const;
        Order ReadBoxingAnimationOrder(std::ifstream& input) const;

        // FileUi
        void ReadScale(std::ifstream& input, DataUiElement& data) const;
        void ReadScaleAxis(std::ifstream& input, DataUiElement& data) const;
        void ReadAnchorX(std::ifstream& input, DataUiElement& data) const;
        void ReadAnchorY(std::ifstream& input, DataUiElement& data) const;
        void ReadPaddingX(std::ifstream& input, DataUiElement& data) const;
        void ReadPaddingY(std::ifstream& input, DataUiElement& data) const;
        void ReadText(std::ifstream& input, DataUiElement& data) const;
        void ReadTextSize(std::ifstream& input, DataUiElement& data) const;
        
        // File
        DataMapElement ReadMapElement(std::ifstream& input) const;
        DataNPC ReadDataNPC(std::ifstream& input) const;
        void ReadHeaderMapFile(std::ifstream& input, DataMap& m) const;

    public:
        FileReader() = default;

        // File
        std::unordered_map<unsigned int, MoveDefinition> ReadMoveFile(const std::string& moveFilepath) const;
        std::vector<DataBattleActor> ReadBattleFile(const std::string& battleFilepath) const;

        std::vector<DataNPC> GetDataNPCsForMap(const std::string& npcsFilepath, const unsigned int mapIndex) const;
        std::optional<DataNPC> GetDataNPCFromId(const std::string& npcFilepath, const unsigned int targetId) const;
        
        std::optional<DataWorld> ReadWorldFile(const std::string& worldFilepath) const;
        std::optional<DataMap> ReadMapFile(const std::string& mapFilepath, Camera& camera, TextureController& textureController,
            Tileset& tileset) const;
        std::optional<DataTileset> ReadTilesetFile(const std::string& path) const;
        std::optional<DataSpriteAnimation> ReadAnimationFile(const std::string& animationFilepath) const;

        void SaveMapFile(const std::string& mapFilepath, const DataMap& dataMap) const;

        // FileOrder
        std::vector<Order> ReadCinematicFile(const std::string& cinematicFilepath) const;
        
        // FileUi
        std::vector<DataUiElement> ReadUiFile(const std::string& uiFilepath) const; // Can be used for UiFile AND UiTemplate
        
        // FileIO
        std::string GetFileExtension(const std::string& filepath) const;
        bool CheckFileExtension(const std::string& filepath, const std::string_view extension) const;
};

using OrderReader = Order(FileReader::*)(std::ifstream&) const;
using UiReader = void(FileReader::*)(std::ifstream&, DataUiElement&) const;
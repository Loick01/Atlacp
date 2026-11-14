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
        std::string ReadString(std::ifstream& input) const;

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
        FrameTextOrder ReadFrameTextOrder(std::ifstream& input) const;
        DialogTextOrder ReadDialogTextOrder(std::ifstream& input) const;
        NpcGoToOrder ReadNpcGoToOrder(std::ifstream& input) const;
        NpcFollowOrder ReadNpcFollowOrder(std::ifstream& input) const;
        NpcIdleOrder ReadNpcIdleOrder(std::ifstream& input) const;
        PlayCinematicOrder ReadPlayCinematicOrder(std::ifstream& input) const;
        CameraSlideToPositionOrder ReadCameraSlideToPositionOrder(std::ifstream& input) const;
        CameraSlideToEntityOrder ReadCameraSlideToEntityOrder(std::ifstream& input) const;
        CameraAnchorEntityOrder ReadCameraAnchorEntityOrder(std::ifstream& input) const;
        EntityOrientationOrder ReadEntityOrientationOrder(std::ifstream& input) const;
        EntityCreateOrder ReadEntityCreateOrder(std::ifstream& input) const;
        EntityDeleteOrder ReadEntityDeleteOrder(std::ifstream& input) const;
        TimeDelayOrder ReadTimeDelayOrder(std::ifstream& input) const;
        LoadMapOrder ReadLoadMapOrder(std::ifstream& input) const;
        BoxingAnimationOrder ReadBoxingAnimationOrder(std::ifstream& input) const;
        
        // File
        DataMapElement ReadMapElement(std::ifstream& input) const;
        DataNPC ReadDataNPC(std::ifstream& input, const unsigned int targetId=-1) const;
        void ReadHeaderMapFile(std::ifstream& input, DataMap& m) const;

    public:
        FileReader() = default;

        // File
        std::unordered_map<unsigned int, MoveDefinition> ReadMoveFile(const std::string& moveFilepath) const;
        std::vector<DataBattleActor> ReadBattleFile(const std::string& battleFilepath) const;

        std::vector<DataNPC> GetDataNPCsForMap(const std::string& npcsFilepath, const unsigned int mapIndex) const;
        std::optional<DataNPC> GetDataNPCFromId(const std::string& npcFilepath, const unsigned int idEntity) const;

        std::vector<DataUiElement> ReadUiFile(const std::string& uiFilepath) const; // Can be used for UiFile AND UiTemplate
        
        std::optional<DataWorld> ReadWorldFile(const std::string& worldFilepath) const;
        std::optional<DataMap> ReadMapFile(const std::string& mapFilepath, Camera& camera, TextureController& textureController,
            Tileset& tileset) const;
        std::optional<DataTileset> ReadTilesetFile(const std::string& path) const;
        std::optional<DataSpriteAnimation> ReadAnimationFile(const std::string& animationFilepath) const;

        void SaveMapFile(const std::string& mapFilepath, const DataMap& dataMap) const;

        // FileOrder
        std::vector<Order> ReadCinematicFile(const std::string& cinematicFilepath) const;
        
        // FileIO
        std::string GetFileExtension(const std::string& filepath) const;
        bool CheckFileExtension(const std::string& filepath, const std::string_view extension) const;
};
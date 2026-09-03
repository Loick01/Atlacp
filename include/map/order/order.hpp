#pragma once

#include <string>
#include <variant>
#include <vector>

#include "map/map_types.hpp" // MapPosition

struct FrameTextOrder { // Display text in a frame
    std::vector<std::string> texts;

    std::string GetString() const
    {
        std::string res = "frame_text " + std::to_string(texts.size()) + " ";
        for (const std::string& s : texts)
            res += s + ";";
        return res;
    }
};

// UiDialogBox inherits from UiFrameText
struct DialogTextOrder { // Display text in a frame with a faceset
    std::vector<std::string> texts;
    std::string facePath;

    std::string GetString() const
    {
        std::string res = "dialog_text " + std::to_string(texts.size()) + " " + facePath + " ";
        for (const std::string& s : texts)
            res += s + ";";
        return res;
    }
};

struct NpcGoToOrder { // Gives MapGoToBehaviour to an NPC, which makes it move toward its target position
    // Should be the same Order for Player (for now only NPC) ?
    MapPosition targetPosition;
    unsigned int idNpc;
    // bool isDone; // Useless ?

    std::string GetString() const
    {
        std::string res = "npc_goto " + std::to_string(targetPosition.x) + " " + std::to_string(targetPosition.y) + " " + std::to_string(idNpc);
        return res;
    }
};

struct NpcFollowOrder { // Gives MapFollowBehaviour to an NPC, which makes it follow a MapEntity (Player or NPC)
    // Should be the same Order for Player (for now only NPC) ?
    unsigned int idTrackedEntity;
    unsigned int idNpc;

    std::string GetString() const
    {
        std::string res = "npc_follow " + std::to_string(idTrackedEntity) + " " + std::to_string(idNpc);
        return res;
    }
};

struct NpcIdleOrder { // Gives MapIdleBehaviour to an NPC, which makes it look in a direction and stay still
    // Should be the same Order for Player (for now only NPC) ?
    Direction direction;
    unsigned int idNpc;

    std::string GetString() const
    {
        std::string res = "npc_idle " + GetStringFromDirection(direction) + " " + std::to_string(idNpc);
        return res;
    }
};

struct PlayCinematicOrder { // Execute Orders defined in a file given by cinematicFilepath
    std::string cinematicFilepath;

    std::string GetString() const
    {
        std::string res = "play_cinematic " + cinematicFilepath;
        return res;
    }
};

struct CameraSlideToPositionOrder { // Move the camera to the given position 
    ScenePosition endPosition;

    std::string GetString() const
    {
        std::string res = "camera_slide_to_position " + std::to_string(endPosition.x) + " " + std::to_string(endPosition.y);
        return res;
    }
};

struct CameraSlideToEntityOrder { // Move the camera to the position of the given MapEntity
    unsigned int idEntity;

    std::string GetString() const
    {
        std::string res = "camera_slide_to_entity " + std::to_string(idEntity);
        return res;
    }
};

struct CameraAnchorEntityOrder { // Make the camera follow the given MapEntity
    unsigned int idEntity; 

    std::string GetString() const
    {
        std::string res = "camera_anchor_entity " + std::to_string(idEntity);
        return res;
    }
};

struct EntityOrientationOrder {
    unsigned int idEntity;
    Direction direction;

    std::string GetString() const
    {
        std::string res = "entity_orientation " + std::to_string(idEntity) + " " + GetStringFromDirection(direction);
        return res;
    }
};

struct EntityDeleteOrder {
    unsigned int idEntity;

    std::string GetString() const
    {
        std::string res = "entity_delete " + std::to_string(idEntity);
        return res;
    }
};

// TODO :
// struct AddInventoryOrder {};
// EntityCreateOrder(idEntity)

using Order = std::variant<
    FrameTextOrder, DialogTextOrder,
    NpcGoToOrder, NpcFollowOrder, NpcIdleOrder,
    CameraSlideToPositionOrder, CameraSlideToEntityOrder,
    CameraAnchorEntityOrder,
    EntityOrientationOrder, EntityDeleteOrder,
    PlayCinematicOrder
>;

std::string GetStringDescription(const Order& order); // Used in FileReader::SaveMapFile()
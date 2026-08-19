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
    // Should be the same Order for Player (for now only NPC)
    MapPosition targetPosition;
    unsigned int idNpc;
    // bool isDone; // Useless ?

    std::string GetString() const
    {
        std::string res = "npc_goto " + std::to_string(targetPosition.x) + " " + std::to_string(targetPosition.y) + " " + std::to_string(idNpc);
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

// TODO :
// struct AddInventoryOrder {};
// Orders about camera

using Order = std::variant<
    FrameTextOrder,
    DialogTextOrder,
    NpcGoToOrder,
    PlayCinematicOrder
>;

std::string GetStringDescription(const Order& order); // Used in FileReader::SaveMapFile()
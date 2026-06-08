#pragma once

#include "ui/ui_types.hpp" // UiKey

enum class Team
{
    Ally, Opponent
};

struct DataBattleActor // Rename
{
    UiKey nameId;
    UiKey healthId;
    UiKey spriteId;
    std::string spritePath;
    std::string name;
    Team team;
    unsigned int health;
    unsigned int turnSpeed;
    bool isAiActor;
};
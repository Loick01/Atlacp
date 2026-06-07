#pragma once

#include "ui/ui_types.hpp"

enum class Team
{
    Ally, Opponent
};

struct DataBattleActor // Rename
{
    ElementKey nameId;
    ElementKey healthId;
    ElementKey spriteId;
    std::string spritePath;
    std::string name;
    Team team;
    unsigned int health;
    unsigned int turnSpeed;
    bool isAiActor;
};
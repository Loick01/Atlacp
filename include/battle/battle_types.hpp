#pragma once

#include <string>

enum class Team
{
    Ally, Opponent
};

struct DataBattleActor // Rename
{
    std::string spritePath;
    std::string name;
    Team team;
    unsigned int health;
    unsigned int turnSpeed;
    bool isAiActor;
};
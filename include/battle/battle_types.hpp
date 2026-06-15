#pragma once

#include <string>

enum class CommandType // Rename
{
    Attack, Heal
};

enum class MoveType // Rename
{
    P, M
};

enum class Team
{
    Ally, Opponent
};

struct DataBattleActor
{
    std::string spritePath;
    std::string name;
    Team team;
    unsigned int health;
    unsigned int turnSpeed;
    bool isAiActor;
};

struct MoveDefinition // Rename
{
    std::string name;
    CommandType commandType;
    MoveType moveType;
    unsigned int value; // Rename
};
#pragma once

#include <string>
#include <vector>

enum class CommandType
{
    Attack, Heal
};

enum class MoveType
{
    Physical, Magic
};

enum class Team
{
    Ally, Opponent
};

struct DataBattleActor
{
    std::vector<unsigned int> moveIds;
    std::string spritePath;
    std::string name;
    Team team;
    unsigned int health;
    unsigned int turnSpeed;
    bool isAiActor;
};

struct MoveDefinition
{
    std::string name;
    CommandType commandType;
    MoveType moveType;
    unsigned int baseValue;
    std::string sfxPath;
    std::string animationPath;
};
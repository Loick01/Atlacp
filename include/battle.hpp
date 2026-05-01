#pragma once

#include <string>

#include "type.hpp"

class BattleActor // Battler ?
{
    private:
        std::string m_name;
        unsigned int m_health;
        
    public:
        BattleActor(const std::string name, const unsigned int health);
        
        std::string GetName() const;
        unsigned int GetHealth() const;
};

class BattleController
{
    private:
        BattleActor m_actorA;
        BattleActor m_actorB;
        BattleEventState m_eventState;

    public:
        BattleController(const BattleActor actorA, const BattleActor actorB);

        void SetEventState(const BattleEventState eventState);
        void PlayTurn();
};
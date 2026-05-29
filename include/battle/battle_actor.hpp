#pragma once

#include "ui/ui_element.hpp"

enum class Team 
{
    Ally, Opponent
};

enum class LifeState
{
    Alive, Dead
};

class BattleActor
{
    private:
        UiValue<std::string> m_name; // const ?
        UiValue<unsigned int> m_health;

        const Team m_team;
        LifeState m_lifeState;
        
        const unsigned int m_maxHealth;
        unsigned int m_strength;
        unsigned int m_turnSpeed; // ]0,+inf] The smaller this value is, the more frequently the actor will play a turn
        
    public:
        BattleActor(const Team team, const ElementKey& nameId, const ElementKey& healthId, const std::string name, const unsigned int health);
        
        UiValue<std::string> GetName() const;
        UiValue<unsigned int> GetHealth() const;
        unsigned int GetStrength() const;
        Team GetTeam() const;
        LifeState GetLifeState() const;
        void AddHealth(const unsigned int hp);
        void RemoveHealth(const unsigned int hp);
};
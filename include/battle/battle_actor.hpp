#pragma once

#include "ui/ui_element.hpp"

enum class LifeState
{
    Alive, Dead
};

enum class Team 
{
    Ally, Opponent
};

class BattleActor
{
    private:
        const UiValue<std::string> m_name;
        UiValue<unsigned int> m_health;

        const Team m_team;
        LifeState m_lifeState;
        
        const unsigned int m_maxHealth;
        unsigned int m_strength;
        unsigned int m_turnSpeed; // ]0,+inf] The smaller this value is, the more frequently the actor will play a turn (TODO)
        
    public:
        BattleActor(const Team team, const ElementKey& nameId, const ElementKey& healthId, const std::string name, const unsigned int health); // Should have UiValue as parameters ?
        
        UiValue<std::string> GetName() const;
        UiValue<unsigned int> GetHealth() const;

        Team GetTeam() const;
        LifeState GetLifeState() const;
        unsigned int GetStrength() const;
        
        void AddHealth(const unsigned int hp);
        void RemoveHealth(const unsigned int hp);
};
#pragma once

#include "ui/ui_element.hpp" // Because UiValue

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
        UiValue<std::string> m_spritePath; // Because I need to update the path
        UiValue<unsigned int> m_health;

        const Team m_team;
        LifeState m_lifeState;
        
        const unsigned int m_maxHealth;
        float m_nextTurnTime;
        unsigned int m_strength;
        unsigned int m_turnSpeed; // ]0,+inf] The higher this value is, the more frequently the actor will play a turn
        
    public:
        BattleActor(const Team team, const ElementKey& nameId, const ElementKey& healthId, const ElementKey& pathId,
            const std::string name, const unsigned int health, const unsigned int turnSpeed); // Should have UiValue as parameters ?
        
        UiValue<std::string> GetName() const;
        UiValue<std::string> GetSpritePath() const;
        UiValue<unsigned int> GetHealth() const;

        Team GetTeam() const;
        LifeState GetLifeState() const;
        float GetNextTurnTime() const;
        unsigned int GetStrength() const;
        
        void ComputeNextTurnTime(const float turnTime); // turnTime is the global current time from BattleController
        void SetSpritePath(const std::string& path);
        void AddHealth(const unsigned int hp);
        void RemoveHealth(const unsigned int hp);
};
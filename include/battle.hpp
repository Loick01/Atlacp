#pragma once

#include <string>

#include "notifier.hpp"
#include "type.hpp"

enum class ExitEvent // Can't use SwitchEvent (from scene.hpp) in this file
{
    ExitWin, ExitLost
};

enum class Turn
{
    ActorA, ActorB
};

class BattleActor // Battler ?
{
    private:
        std::string m_name;
        unsigned int m_health;
        unsigned int m_strength;
        
    public:
        BattleActor(const std::string name, const unsigned int health);
        
        std::string GetName() const;
        unsigned int GetHealth() const;
        unsigned int GetStrength() const;

        void TakeDamage(const unsigned int strength);
};

class BattleController : public Notifier<ExitEvent>, public EventStateHolder<BattleEventState>
{
    private:
        BattleActor m_actorA; // Player
        BattleActor m_actorB; // Enemy
        Turn m_currentTurn;

    public:
        BattleController(const BattleActor actorA, const BattleActor actorB);

        void CheckActorHealth();
        void PlayTurn();
};
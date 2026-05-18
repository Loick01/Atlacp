#pragma once

#include <string>

#include "notifier.hpp"
#include "type.hpp"
#include "ui_controller.hpp"
#include "ui_selector.hpp"

enum class ExitEvent // Can't use SwitchEvent (from scene.hpp) in this file
{
    ExitWin, ExitLost
};

enum class Turn // Will be removed
{
    ActorA, ActorB
};

class BattleActor
{
    private:
        UiValue<std::string> m_name;
        UiValue<unsigned int> m_health;

        unsigned int m_strength;
        
    public:
        BattleActor(const ElementKey& nameId, const ElementKey& healthId, const std::string name, const unsigned int health);
        
        ElementKey GetHealthId() const;
        ElementKey GetNameId() const;

        std::string GetName() const;
        unsigned int GetHealth() const;
        unsigned int GetStrength() const;

        void RemoveHealth(const unsigned int damage);
};

class BattleController : public Notifier<ExitEvent>, public EventStateHolder<BattleEventState>
{
    private:
        UiController* m_uiController; // Should be BattleUiController ?
        BattleActor m_actorA; // Player
        BattleActor m_actorB; // Enemy
        UiSelector m_selector;
        Turn m_currentTurn;

    public:
        BattleController(const BattleActor actorA, const BattleActor actorB);

        void SetUiController(UiController* uiController);
        void UpdateStatus(); // Rename
        void CheckActorHealth();

        void InitPlayerTurn(); // Will be removed
        void PlayTurn(BattleActor& source, BattleActor& target);
        void PlayFight();
};
#pragma once

#include <string>

#include "notifier.hpp"
#include "type.hpp"
#include "ui_controller.hpp"
#include "ui_selector.hpp"
#include "ui_text_list.hpp"

enum class ExitEvent // Can't use SwitchEvent (from scene.hpp) in this file
{
    ExitWin, ExitLost
};

enum class Turn // Will be removed
{
    ActorA, WaitingA, ActorB, WaitingB
};

class BattleActor
{
    private:
        UiValue<std::string> m_name;
        UiValue<unsigned int> m_health;

        unsigned int m_strength;
        
    public:
        BattleActor(const ElementKey& nameId, const ElementKey& healthId, const std::string name, const unsigned int health);
        
        UiValue<std::string> GetName() const;
        UiValue<unsigned int> GetHealth() const;
        unsigned int GetStrength() const;

        void ModifyHealth(const int hp);
};

class BattleController : public Notifier<ExitEvent>, public EventStateHolder<BattleEventState>
{
    private:
        UiController& m_uiController; 
        BattleActor m_actorA; // Player
        BattleActor m_actorB; // Enemy
        UiSelector m_selector; // Will be removed, I need a UiList component
        UiTextList m_textList;
        Turn m_currentTurn;

    public:
        BattleController(UiController& uiController, const BattleActor actorA, const BattleActor actorB);

        void UpdateStatus(); // Rename
        void CheckActorHealth();

        void OpenPlayerOption(); // Will be removed
        void ClosePlayerOption(); // Will be removed
        unsigned int TakeDamage(BattleActor& source, BattleActor& target); // Rename
        unsigned int TakeHealth(BattleActor& source); // Rename
        void HandlePlayerSelection(const int index); // Rename
        void HandleEnemyTurn(); // Will be removed, I will use behaviour class
        void PlayFight();
};
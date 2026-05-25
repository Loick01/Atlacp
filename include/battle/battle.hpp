#pragma once

// #include <queue>
#include <string>

#include "system/notifier.hpp"
#include "system/type.hpp"
#include "ui/ui_controller.hpp"
#include "ui/ui_selector.hpp"
#include "ui/ui_text_series.hpp"

enum class ExitEvent // Can't use SwitchEvent (from scene.hpp) in this file
{
    ExitWin, ExitLost
};

enum class Turn
{
    Init, OptionSelection, ActorSelection, Waiting
};

enum class Team 
{
    Ally, Opponent
};

class BattleActor
{
    private:
        UiValue<std::string> m_name; // const ?
        UiValue<unsigned int> m_health;
        
        // unsigned int maxHealth;
        const Team m_team;
        unsigned int m_strength;
        unsigned int m_turnSpeed; // ]0,+inf] The smaller this value is, the more frequently the actor will play a turn
        
    public:
        BattleActor(const Team team, const ElementKey& nameId, const ElementKey& healthId, const std::string name, const unsigned int health);
        
        UiValue<std::string> GetName() const;
        UiValue<unsigned int> GetHealth() const;
        unsigned int GetStrength() const;
        Team GetTeam() const;
        void ModifyHealth(const int hp);
};

class BattleController : public Notifier<ExitEvent>, public EventStateHolder<BattleEventState>
{
    private:
        std::vector<BattleActor> m_actors;
        std::queue<BattleActor*> m_turns; // Used to store turn order
        BattleActor* m_currentActor; // Actor that is playing his turn

        UiController& m_uiController; 
        UiSelector m_selector;
        UiTextSeries m_textList;
        Turn m_currentTurn;

    public:
        BattleController(UiController& uiController);

        BattleActor* GetNextTurn();
        void UpdateStatus(); // Rename
        bool HasAliveActor(const Team team);
        void CheckActorHealth();

        void OpenPlayerOption(); // Will be removed
        void ClosePlayerOption(); // Will be removed
        unsigned int TakeDamage(BattleActor& source, BattleActor& target); // Rename
        unsigned int TakeHealth(BattleActor& source); // Rename
        void HandleOptionSelection(BattleActor& srcActor, const int index);
        void HandleActorSelection();
        void ApplyDamage(BattleActor& srcActor, const int selectorIndex);
        void HandleOpponentTurn(BattleActor& srcActor); // Will be removed, I will use behaviour class
        void PlayNextTurn();
};
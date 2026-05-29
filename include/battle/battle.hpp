#pragma once

// #include <queue>
#include <string>

#include "system/notifier.hpp"
#include "system/type.hpp"
#include "ui/ui_controller.hpp"
#include "ui/ui_dynamic_list.hpp"
#include "ui/ui_list.hpp"
#include "ui/ui_selector.hpp"
#include "ui/ui_text_series.hpp"

enum class ExitEvent // Can't use SwitchEvent (from scene.hpp) in this file
{
    ExitWin, ExitLost
};

enum class Turn
{
    Init, MoveSelection, ActorSelection, Waiting
};

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

class BattleController : public Notifier<ExitEvent>, public EventStateHolder<BattleEventState>
{
    private:
        std::vector<BattleActor> m_actors;
        std::queue<BattleActor*> m_turns; // Used to store turn order
        BattleActor* m_currentActor; // Actor that is playing his turn

        UiController& m_uiController; 
        UiSelector m_selector;
        UiTextSeries m_textSeries;
        UiDynamicList m_allyList;
        UiDynamicList m_opponentList;
        UiList m_allyMoveList;
        Turn m_currentTurn; // TODO

    public:
        BattleController(UiController& uiController);

        BattleActor* GetNextTurn();
        void InitializeActors();
        bool HasAliveActor(const Team team);
        void CheckActorsHealth();

        void OpenAllyMoveSelection();
        void CloseAllyMoveSelection();
        unsigned int TakeDamage(BattleActor& source, BattleActor& target); // Rename
        unsigned int TakeHealth(BattleActor& source); // Rename
        void HandleAllyMoveSelection(BattleActor& srcActor, const int index);
        void HandleActorSelection(); // I should have Open/CloseActorSelection() ?
        void ApplyDamage(BattleActor& srcActor, BattleActor& targetActor);
        void ApplyHealth(BattleActor& srcActor);
        void HandleOpponentMoveSelection(BattleActor& srcActor); // Will be removed, I will use behaviour class
        void PlayNextTurn();
};
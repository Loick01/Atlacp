#pragma once

#include <queue>
#include <string>

#include "battle/battle_actor.hpp"
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
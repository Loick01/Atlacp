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
    ExitWin, ExitLost, None
};

enum class Turn
{
    Init, MoveSelection, ActorSelection, Waiting, End
};

class BattleController : public Notifier<ExitEvent>, public EventStateHolder<BattleEventState>
{
    private:
        std::vector<BattleActor> m_actors;
        std::queue<BattleActor*> m_turns; // Used to store turn order
        BattleActor* m_currentActor; // Actor that is playing his turn
        Turn m_currentTurn;
        ExitEvent m_exitEvent;
        
        UiController& m_uiController;
        UiDynamicList m_allyList;
        UiDynamicList m_opponentList;
        UiList m_allyMoveList; 
        UiSelector m_selector;
        UiTextSeries m_textSeries;

        BattleActor* PopNextTurn(); // Return (and remove) the next actor in front of m_turns
        ExitEvent CheckBattleEnd() const;
        unsigned int ComputeDamage(BattleActor& source, BattleActor& target);
        unsigned int ComputeHeal(BattleActor& source);
        bool HasAliveActor(const Team team) const;
        
        void ApplyDamage(BattleActor& srcActor, BattleActor& targetActor);
        void ApplyHeal(BattleActor& srcActor); // Will have a targetActor parameter

        void OpenAllyMoveSelection();
        void CloseAllyMoveSelection();
        void HandleAllyMoveSelection(BattleActor& srcActor, const int index);
        void HandleOpponentMoveSelection(BattleActor& srcActor); // Behaviours will be used in this function
        
        void HandleActorSelection(); // I should have Open/CloseActorSelection() ? (for now target selection is only used when an ally attacks an opponent)

    public:
        BattleController(UiController& uiController);
        
        void InitializeActors();
        void PlayNextTurn();
};
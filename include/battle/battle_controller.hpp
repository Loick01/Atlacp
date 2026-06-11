#pragma once

#include <memory>
#include <queue>
#include <string>

#include "battle/battle_actor.hpp"
#include "battle/battle_types.hpp" // Team
#include "core/notifier.hpp"
#include "event/event_types.hpp" // BattleEventState
#include "ui/ui_dynamic_list.hpp"
#include "ui/ui_list.hpp"
#include "ui/ui_selector.hpp"
#include "ui/ui_text_series.hpp"

class AiActor;
class FileReader;
class UiController;

enum class BattleCommand // Could be a struct ?
{
    Attack, Heal
};

enum class ExitEvent // Can't use SwitchEvent (from scene.hpp) in this file
{
    ExitWin, ExitLost, None
};

enum class TurnState
{
    Init, MoveSelection, ActorSelection, Waiting, End
};

struct TurnComparer
{
    bool operator()(const BattleActor* b1, const BattleActor* b2) const
    {
        return b1->GetNextTurnTime() > b2->GetNextTurnTime();
    }
};

class BattleController : public Notifier<ExitEvent>, public EventStateHolder<BattleEventState>
{
    private:
        std::vector<std::unique_ptr<BattleActor>> m_actors;
        std::priority_queue<BattleActor*, std::vector<BattleActor*>, TurnComparer> m_turns; // Used to store turn order
        BattleActor* m_currentActor; // Actor that is playing his turn
        // BattleActor* m_targetActor;
        TurnState m_turnState;
        ExitEvent m_exitEvent;
        BattleCommand m_currentCommand;
        float m_currentTime;
        
        FileReader& m_fileReader;

        UiController& m_uiController;
        UiDynamicList m_allyList;
        UiDynamicList m_opponentList;
        UiList m_allyMoveList; 
        UiSelector m_selector;
        UiTextSeries m_textSeries;

        std::vector<BattleActor*> GetActorsInTeam(const Team team) const;
        std::vector<BattleActor*> FilterActorsByLifeState(std::vector<BattleActor*> actors, const LifeState lifeState) const;
        
        BattleActor* PopNextTurn(); // Return (and remove) the next actor in front of m_turns
        BattleActor* GetActorSelection(); // I should have Open/CloseActorSelection() ?
        ExitEvent CheckBattleEnd() const;
        Team GetTeamFromCommand() const; // Remove ?
        unsigned int ComputeDamage(BattleActor& source, BattleActor& target);
        unsigned int ComputeHeal(BattleActor& source,  BattleActor& target);
        bool HasAliveActor(const Team team) const;
        
        void ApplyDamage(BattleActor& srcActor, BattleActor& targetActor);
        void ApplyHeal(BattleActor& srcActor, BattleActor& targetActor);

        void OpenAllyMoveSelection();
        void CloseAllyMoveSelection();
        void HandleActorMoveSelection(const int index);
        void HandleAiActorMoveSelection(AiActor& srcActor);
        
        void HandleCurrentCommand(BattleActor* targetActor);

    public:
        BattleController(FileReader& fileReader, UiController& uiController);
        
        void InitializeActors(const std::string& battleFile); // Rename battleFile
        void PlayNextTurn();
};
#pragma once

#include <memory>
#include <queue>
#include <string>

#include "battle/battle_actor.hpp"
#include "battle/battle_types.hpp" // Team
#include "core/logger.hpp"
#include "core/notifier.hpp"
#include "event/event_types.hpp" // BattleEventState
#include "ui/component/ui_dynamic_list.hpp"
#include "ui/component/ui_list.hpp"
#include "ui/component/ui_selector.hpp"
#include "ui/component/ui_sprite_animation.hpp"
#include "ui/component/ui_frame_text.hpp"

class AiActor;
class FileReader;
class UiComponentController;
class UiController;

enum class ExitEvent
{
    ExitWin, ExitLost, None
};

enum class TurnState
{
    Init, 
    ActionSelection, AiActionSelection,
    MoveSelection, AiMoveSelection,
    ActorSelection, AiActorSelection,
    HandleCommand, WaitingForText, WaitingForAnimation, End
};

struct BattleCommand
{
    CommandType commandType;
    Team sourceTeam;
    Team targetTeam;
    BattleState targetBattleState;
    unsigned int amount;
    std::string sfx;
    std::string animation;

    Team ComputeTargetTeam() const
    {
        switch (commandType) {
            case CommandType::Attack :
                return (sourceTeam == Team::Ally ? Team::Opponent : Team::Ally);
            case CommandType::Heal :
                return sourceTeam;
            default :
                Logger::GetInstance().AddLog(Level::L4, "Unhandled CommandType used in BattleCommand::ComputeTargetTeam"); // TODO : Level
                return sourceTeam; // TODO : Maybe I could find something else to return ?
        }
    }

    BattleCommand() = default;

    BattleCommand(const CommandType ct, const Team st, const BattleState s, const unsigned int a, const std::string& sfxPath, const std::string& animationPath) {
        commandType = ct;
        sourceTeam = st;
        targetTeam = ComputeTargetTeam();
        targetBattleState = s;
        amount = a;
        sfx = sfxPath;
        animation = animationPath;
    }
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
        std::vector<BattleActor*> m_allies; std::vector<BattleActor*> m_opponents;
        BattleActor* m_currentActor; // Actor that is playing his turn
        BattleActor* m_targetActor;
        TurnState m_turnState;
        ExitEvent m_exitEvent;
        BattleCommand m_currentCommand;
        float m_globalTurnTime;
        
        UiComponentController& m_uiComponentController;
        UiController& m_uiController;
        FileReader& m_fileReader;

        std::vector<BattleActor*> GetActorsInTeam(const Team team) const; // TODO : Should be FilterActorsByTeam(const std::vector<BattleActor*>&, const Team) const ?
        std::vector<BattleActor*> FilterActorsByBattleState(const std::vector<BattleActor*>& actors, const BattleState lifeState) const;
        bool HasAliveActorsInTeam(const Team team) const;

        BattleActor* PopNextTurn(); // Return (and remove) the next actor in front of m_turns
        ExitEvent CheckBattleEnd() const;

        BattleCommand CreateCommand(const BattleActor* srcActor, const MoveDefinition& md) const;
        unsigned int ComputeAmount(const MoveType mt, const unsigned baseValue, const BattleActor* srcActor) const;
        
        void ApplyDamage(BattleActor& srcActor, BattleActor& targetActor);
        void ApplyHeal(BattleActor& srcActor, BattleActor& targetActor);

        void OpenActionSelection(); 
        void CloseActionSelection();
        void OpenMoveSelection(); 
        void CloseMoveSelection();
        void OpenActorSelection();
        void CloseActorSelection();
        
        void HandleActionSelection(const int index);
        void HandleMoveSelection(const int moveIndex);
        BattleActor* HandleActorSelection(); // TODO : void

        void HandleActionSelectionAi(AiActor& srcActor);
        void HandleMoveSelectionAi(AiActor& srcActor);
        void HandleActorSelectionAi(AiActor& srcActor);
        
        void HandleCurrentCommand();
        void EndTurn();

    public:
        BattleController(FileReader& fileReader, UiComponentController& uiComponentController, UiController& uiController);
        
        void InitializeActors(const std::string& dataFile);
        void PlayNextTurn();
};
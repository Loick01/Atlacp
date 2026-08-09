#pragma once

#include <memory>
#include <queue>
#include <string>

#include "battle/battle_actor.hpp"
#include "battle/battle_types.hpp" // Team
#include "core/notifier.hpp"
#include "event/event_types.hpp" // BattleEventState
#include "ui/component/ui_dynamic_list.hpp"
#include "ui/component/ui_list.hpp"
#include "ui/component/ui_selector.hpp"
#include "ui/component/ui_sprite_animation.hpp"
#include "ui/component/ui_text_series.hpp"

struct Time; // Remove
class AiActor;
class FileReader;
class UiController;

enum class ExitEvent // Can't use SwitchEvent (from scene.hpp) in this file
{
    ExitWin, ExitLost, None
};

enum class TurnState
{
    Init, ActionSelection, MoveSelection, ActorSelection, HandleCommand, WaitingForText, WaitingForAnimation, End
};

struct BattleCommand
{
    CommandType commandType;
    Team sourceTeam;
    Team targetTeam;
    LifeState targetLifeState;
    unsigned int moveValue; // Rename
    std::string sfx;
    std::string animation; // Remove ?

    Team ComputeTargetTeam() const
    {
        switch (commandType) {
            case CommandType::Attack :
                return (sourceTeam == Team::Ally ? Team::Opponent : Team::Ally);
            case CommandType::Heal :
                return sourceTeam;
            default :
                throw std::runtime_error("Unknown CommandType value"); 
        }
    }

    BattleCommand() = default;

    BattleCommand(const CommandType ct, const Team st, const LifeState s, const unsigned int mv, const std::string& sfxPath, const std::string& animationPath) {
        commandType = ct;
        sourceTeam = st;
        targetTeam = ComputeTargetTeam();
        targetLifeState = s;
        moveValue = mv;
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
        float m_currentTime; // Rename
        
        FileReader& m_fileReader;

        UiController& m_uiController;
        UiDynamicList m_allyList;
        UiDynamicList m_opponentList;
        UiList m_staticList; // Used for action selection
        UiDynamicList m_dynamicList; // Used for move selection
        UiSelector m_selector;
        UiTextSeries m_textSeries;
        UiSpriteAnimation m_moveAnimation; // Should be in BattleCommand ?

        std::vector<BattleActor*> GetActorsInTeam(const Team team) const; // Not a Filter... function. Return either m_allies or m_opponents
        std::vector<BattleActor*> FilterActorsByLifeState(std::vector<BattleActor*> actors, const LifeState lifeState) const;
        
        BattleActor* PopNextTurn(); // Return (and remove) the next actor in front of m_turns
        BattleActor* GetActorSelection();
        ExitEvent CheckBattleEnd() const;
        BattleCommand CreateCommand(const BattleActor* srcActor, const MoveDefinition& md) const; // Should remove srcActor ?
        unsigned int ComputeMoveValue(const MoveType mt, const unsigned baseValue, const BattleActor* srcActor) const;
        bool HasAliveActor(const Team team) const;
        
        void ApplyDamage(BattleActor& srcActor, BattleActor& targetActor);
        void ApplyHeal(BattleActor& srcActor, BattleActor& targetActor);

        void OpenActionSelection(); 
        void CloseActionSelection();
        void OpenMoveSelection(); 
        void CloseMoveSelection();
        void OpenSelectorOnActors();
        void HandleActionSelection(const int index);
        void HandleAiActionSelection(AiActor& srcActor); // Rename ?
        void HandleMoveSelection(const int moveIndex);
        
        void HandleCurrentCommand();

    public:
        BattleController(const Time& time, FileReader& fileReader, UiController& uiController);
        
        void InitializeActors(const std::string& battleFile); // Rename battleFile
        void PlayNextTurn();
};
#include "battle/battle_controller.hpp"

#include "ai_battle/battle_behaviour.hpp"
#include "battle/ai_actor.hpp"
#include "sound/sound.hpp"
#include "ui/component/ui_component_controller.hpp"
#include "ui/element/ui_controller.hpp"

void BattleController::HandleInit()
{
    m_currentActor = PopNextTurn();
    if (m_currentActor == nullptr) {
        // This can happen only if m_turns is empty (because of the while loop in PopNextTurn()). I have no choice but to exit the battle
        // There is already a Log in PopNextTurn() in this situation
        m_exitEvent = ExitEvent::ExitLost;
        SetTurnState(TurnState::End);
    }

    if (dynamic_cast<AiActor*>(m_currentActor) == nullptr) 
        SetTurnState(TurnState::ActionSelection);
    else
        SetTurnState(TurnState::ActionSelectionAi);
}

void BattleController::HandleCurrentCommand()
{
    SoundController::GetInstance().RequestChunk(m_currentCommand.sfx); // Request here, thus if ApplyDamage make a new request, the move sfx will not be played

    // TODO : Should call CreateSpriteAnimation() here, with animationPath and targetElement (instead of using setters) ?
    UiSpriteAnimation* moveAnimation = dynamic_cast<UiSpriteAnimation*>(m_uiComponentController.GetComponent("moveAnimation"));
    moveAnimation->SetAnimationPath(m_currentCommand.animation);
    moveAnimation->SetTargetElement(m_targetActor->GetSpritePath().id);
    moveAnimation->Open();

    switch (m_currentCommand.commandType) {
        case CommandType::Attack: {
            ApplyDamage(*m_currentActor, *m_targetActor);
            break;
        }
        case CommandType::Heal: {
            ApplyHeal(*m_currentActor, *m_targetActor);
            break;
        }
        default:
            Logger::GetInstance().AddLog(Level::L4, "Unhandled CommandType used in BattleController::HandleCurrentCommand"); // TODO : Level
            moveAnimation->Close(); // Cancel the move animation
            SoundController::GetInstance().ClearChunk(); // Cancel the move sfx
            SetTurnState(TurnState::Init);
    }

    SetTurnState(TurnState::WaitingForAnimation);
}

void BattleController::HandleActionSelection(const int actionIndex)
{
    CloseActionSelection();
    
    switch (actionIndex) {
        case 0: {
            SetTurnState(TurnState::MoveSelection);
            break;
        }

        case 1: {
            UiFrameText* frameText = dynamic_cast<UiFrameText*>(m_uiComponentController.GetComponent("frameText"));
            frameText->Open();
            frameText->AddText({"This turn has been skipped"});
            frameText->NextText();
            SetTurnState(TurnState::WaitingForText);
            break;
        }
        
        case 2: {
            m_exitEvent = ExitEvent::ExitLost;
            SetTurnState(TurnState::End);
            break;
        }

        default:
            Logger::GetInstance().AddLog(Level::L4, "Selector is not supposed to be on this index : " + std::to_string(actionIndex)); // TODO : Level
            SetTurnState(TurnState::Init);
            break;
    }
}

void BattleController::HandleMoveSelection(const int moveIndex)
{
    CloseMoveSelection();
    
    const MoveDefinition md = m_currentActor->GetMove(moveIndex);
    m_currentCommand = CreateCommand(m_currentActor, md);

    SetTurnState(TurnState::ActorSelection);
}

void BattleController::HandleActorSelection(const int actorIndex)
{
    CloseActorSelection();

    std::vector<BattleActor*> actors = FilterActorsByBattleState(GetActorsInTeam(m_currentCommand.targetTeam), m_currentCommand.targetBattleState);
    m_targetActor = actors.at(actorIndex); // This should not fail, because the selector keys are initialized with the same filtered BattleActors in OpenActorSelection()

    SetTurnState(TurnState::HandleCommand);
}

void BattleController::HandleActionSelectionAi(AiActor& srcActor)
{
    BattleBehaviour& srcBehaviour = srcActor.GetBehaviour();
    const AiAction action = srcBehaviour.ChooseAiAction();

    switch (action) {
        case AiAction::MakeMove: {
            SetTurnState(TurnState::MoveSelectionAi);
            break;
        }
        case AiAction::Flee: {
            UiFrameText* frameText = dynamic_cast<UiFrameText*>(m_uiComponentController.GetComponent("frameText"));
            frameText->Open();
            frameText->AddText({srcActor.GetName().value + " has fled"});
            frameText->NextText();

            srcActor.SetBattleState(BattleState::Fled);
            m_uiController.DeleteElement(srcActor.GetSpritePath().id);  // SpritePath.id is the parent key of each item in allyList/opponentList
            
            SetTurnState(TurnState::WaitingForText);
            break;
        }
        default:
            Logger::GetInstance().AddLog(Level::L4, "Unhandled AiAction used in BattleController::HandleActionSelectionAi"); // TODO : Level
            SetTurnState(TurnState::Init);
            break;
    }
}

void BattleController::HandleMoveSelectionAi(AiActor& srcActor)
{
    BattleBehaviour& srcBehaviour = srcActor.GetBehaviour();
    const MoveDefinition aiMove = srcBehaviour.GetAiMove(srcActor.GetMoves());
    m_currentCommand = CreateCommand(m_currentActor, aiMove); // m_currentActor is srcActor

    SetTurnState(TurnState::ActorSelectionAi);
}

void BattleController::HandleActorSelectionAi(AiActor& srcActor)
{
    BattleBehaviour& srcBehaviour = srcActor.GetBehaviour();
    const BattleState lf = m_currentCommand.targetBattleState;
    m_targetActor = srcBehaviour.SelectTarget(FilterActorsByBattleState(GetActorsInTeam(m_currentCommand.targetTeam), lf));

    SetTurnState(TurnState::HandleCommand);
}
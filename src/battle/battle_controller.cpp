#include "battle/battle_controller.hpp"

#include "ai_battle/battle_behaviour.hpp"
#include "battle/ai_actor.hpp"
#include "core/file/file.hpp"
#include "sound/sound.hpp"
#include "ui/component/ui_component_controller.hpp"
#include "ui/element/ui_controller.hpp"

namespace { // These values must be the same as in the template file used for the UiElement associated to a BattleActor (in BattleController::InitializeActors())
    // Should be constexpr std::string_view ?
    const std::string PrefixName = "actorName";
    const std::string PrefixHealth = "actorHealth";
    const std::string PrefixSprite = "actorSprite";
}

BattleController::BattleController(FileReader& fileReader, UiComponentController& uiComponentController, UiController& uiController):
    m_uiComponentController(uiComponentController), m_uiController(uiController), m_fileReader(fileReader), 
    m_currentActor(nullptr), m_targetActor(nullptr), m_turnState(TurnState::Init), m_exitEvent(ExitEvent::None),
    m_currentTime(0.f)
{
    // Instead of using CreateDynamic, CreateSelector, etc. each time I need a UiComponent and DeleteComponent when I don't need 
    // it anymore (until next time), I call these functions once here, and then I only use UiComponent::Open()/Close()
    // I could do either one, but I prefer this solution  (for now)
    m_uiComponentController.CreateDynamicList("moveSelection", "move_text.uit");
    m_uiComponentController.CreateList("actionSelectionList", "battle_action_selection.uif");
    m_uiComponentController.CreateSelector("selector", "selector.uit"); // Should have optionKey in its constructor ?
    m_uiComponentController.CreateFrameText("frameText", "frame_text.uif");
    m_uiComponentController.CreateSpriteAnimation("moveAnimation", "move_animation.uit"); // Could have animationPath and targetElement in its constructor ? 
}

BattleController::~BattleController()
{
    m_uiComponentController.DeleteAll();
}

std::vector<BattleActor*> BattleController::GetActorsInTeam(const Team team) const
{
    switch(team) {
        case Team::Ally : 
            return m_allies;
        case Team::Opponent :
            return m_opponents;
        default :
            throw std::runtime_error("Unknown Team value");
    }
}

std::vector<BattleActor*> BattleController::FilterActorsByLifeState(std::vector<BattleActor*> actors, const LifeState lifeState) const
{
    std::vector<BattleActor*> result;
    for (BattleActor* b : actors) {
        if (b->GetLifeState() == lifeState)
            result.push_back(b);
    }
    return result;
}

BattleActor* BattleController::PopNextTurn()
{
    BattleActor* actor = nullptr;
    while (actor == nullptr) { // Should try m_turns.empty() (even if it's not possible) ?
        actor = m_turns.top();
        m_turns.pop();
        if (actor->GetLifeState() == LifeState::Dead) // When a BattleActor dies, he's not instantly removed from m_turns 
            actor = nullptr; // Dead actor is not pushed in m_turns
    }
    m_currentTime = actor->GetNextTurnTime();
    actor->ComputeNextTurnTime(m_currentTime);
    m_turns.push(actor);
    return actor;
}

BattleActor* BattleController::GetActorSelection() // Could return BattleActor index in m_actors ?
{
    UiSelector* selector = dynamic_cast<UiSelector*>(m_uiComponentController.GetComponent("selector"));
    if (selector->VerticalNavigation(m_eventState.uiDirection, m_eventState.isAction)) {
        std::vector<BattleActor*> actors = FilterActorsByLifeState(GetActorsInTeam(m_currentCommand.targetTeam), m_currentCommand.targetLifeState);
        BattleActor* targetActor = actors[selector->GetOptionIndex()];
        return targetActor;
    }
    return nullptr;
}

ExitEvent BattleController::CheckBattleEnd() const
{
    if (!HasAliveActor(Team::Ally))
        return ExitEvent::ExitLost;
    else if (!HasAliveActor(Team::Opponent))
        return ExitEvent::ExitWin;
    return ExitEvent::None;
}

unsigned int BattleController::ComputeMoveValue(const MoveType mt, const unsigned baseValue, const BattleActor* srcActor) const
{
    switch(mt) {
        case MoveType::Physical :
            return baseValue * srcActor->GetStrength();
        case MoveType::Magic :
            return baseValue * srcActor->GetStrength(); // TODO : Need a new field in BattleActor
        default:
            throw std::runtime_error("Unknown MoveType value");
    }
}

BattleCommand BattleController::CreateCommand(const BattleActor* srcActor, const MoveDefinition& md) const
{
    const unsigned int amount = ComputeMoveValue(md.moveType, md.value, srcActor);
    return BattleCommand(md.commandType, srcActor->GetTeam(), LifeState::Alive, amount, md.sfxPath, md.animationPath); // Will not be LifeState::Alive
}

bool BattleController::HasAliveActor(const Team team) const
{
    const std::vector<BattleActor*> actors = GetActorsInTeam(team);
    return (FilterActorsByLifeState(actors, LifeState::Alive)).size() > 0;
}

void BattleController::ApplyDamage(BattleActor& srcActor, BattleActor& targetActor)
{
    const unsigned int damage = m_currentCommand.moveValue;
    targetActor.RemoveHealth(damage);
    m_uiController.UpdateText(targetActor.GetHealth());
    
    UiFrameText* frameText = dynamic_cast<UiFrameText*>(m_uiComponentController.GetComponent("frameText"));
    frameText->Open();
    frameText->AddText({srcActor.GetName().value + " attacks " + targetActor.GetName().value + " !",
                        targetActor.GetName().value + " lost " + std::to_string(damage) + " HP !"});
    
    if (targetActor.GetLifeState() == LifeState::Dead) {
        frameText->AddText({targetActor.GetName().value + " fainted !"});
        targetActor.SetSpritePath("battle/gravestone");   
        m_uiController.UpdatePath(targetActor.GetSpritePath());
        SoundController::GetInstance().RequestChunk(BaseSfx::Death);
    }
    
    frameText->NextText(); // Should not be here ?
}

void BattleController::ApplyHeal(BattleActor& srcActor, BattleActor& targetActor)
{
    const unsigned int hp = m_currentCommand.moveValue;
    targetActor.AddHealth(hp);
    m_uiController.UpdateText(targetActor.GetHealth());
    
    UiFrameText* frameText = dynamic_cast<UiFrameText*>(m_uiComponentController.GetComponent("frameText"));
    frameText->Open();
    frameText->AddText({srcActor.GetName().value + " gives a potion to " + targetActor.GetName().value + ".",
                          targetActor.GetName().value + " recovered " + std::to_string(hp) + " HP !"});

    frameText->NextText();
}

void BattleController::OpenActionSelection()
{
    UiList* actionSelectionList = dynamic_cast<UiList*>(m_uiComponentController.GetComponent("actionSelectionList"));
    UiSelector* selector = dynamic_cast<UiSelector*>(m_uiComponentController.GetComponent("selector"));

    actionSelectionList->Open();
    selector->Open();
    selector->SetOptionKeys(actionSelectionList->GetItemsKey(), Axis::Height, Axis::Width, 1.f, -0.05f);
}

void BattleController::CloseActionSelection()
{
    m_uiComponentController.CloseComponent("selector");
    m_uiComponentController.CloseComponent("actionSelectionList");
}

void BattleController::OpenMoveSelection()
{
    const std::vector<MoveDefinition>& moves = m_currentActor->GetMoves();

    UiDynamicList* moveSelection = dynamic_cast<UiDynamicList*>(m_uiComponentController.GetComponent("moveSelection"));
    moveSelection->SetFirstItemParams(
        UiParams(m_uiController.GetResultFromPartialSize(PartialSize("frame", Axis::Width, 0.5f)), Axis::Width, // Scale
        Anchor::LeftIn, Anchor::TopIn, // Anchor
        m_uiController.GetResultFromPartialSize(PartialSize("frame", Axis::Width, 0.06f)), // Padding
        m_uiController.GetResultFromPartialSize(PartialSize("frame", Axis::Height, 0.25f))));
    moveSelection->SetNrItem(moves.size());
    moveSelection->Open();
    
    const std::vector<UiKey> keys = moveSelection->GetItemsKey();
    for (unsigned int i = 0 ; i < keys.size() ; i++) // keys.size = moves.size()
        m_uiController.UpdateText(keys[i], moves[i].name);

    UiSelector* selector = dynamic_cast<UiSelector*>(m_uiComponentController.GetComponent("selector"));
    selector->Open();
    selector->SetOptionKeys(keys, Axis::Height, Axis::Width, 1.f, -0.05f);
}

void BattleController::CloseMoveSelection()
{
    m_uiComponentController.CloseComponent("selector");
    m_uiComponentController.CloseComponent("moveSelection");
}

void BattleController::OpenActorSelection()
{
    UiSelector* selector = dynamic_cast<UiSelector*>(m_uiComponentController.GetComponent("selector"));
    selector->Open();
    std::vector<BattleActor*> aliveActors = FilterActorsByLifeState(GetActorsInTeam(m_currentCommand.targetTeam), m_currentCommand.targetLifeState);
    std::vector<UiKey> keys;
    for (const BattleActor* actor : aliveActors)
        keys.push_back(actor->GetSpritePath().id); // SpritePath.id is the parent key of each item in allyList/opponentList
    selector->SetOptionKeys(keys, Axis::Height, Axis::Width, 0.2f, 0.2f);

    UiFrameText* frameText = dynamic_cast<UiFrameText*>(m_uiComponentController.GetComponent("frameText"));
    frameText->Open();
    frameText->AddText({"Select a BattleActor"});
    frameText->NextText();
}

void BattleController::CloseActorSelection()
{
    m_uiComponentController.CloseComponent("frameText");
    m_uiComponentController.CloseComponent("selector");
}

void BattleController::HandleActionSelection(const int selectorIndex)
{
    CloseActionSelection();
    
    switch (selectorIndex) {
        case 0: {
            OpenMoveSelection();
            m_turnState = TurnState::MoveSelection; // Should be in OpenMoveSelection() ?
            break;
        }

        case 1: {
            UiFrameText* frameText = dynamic_cast<UiFrameText*>(m_uiComponentController.GetComponent("frameText"));
            frameText->Open();
            frameText->AddText({"This turn has been skipped"});
            frameText->NextText();
            m_turnState = TurnState::WaitingForText;
            break;
        }
        
        case 2: {
            m_exitEvent = ExitEvent::ExitLost;
            m_turnState = TurnState::End;
            break;
        }

        default:
            throw std::runtime_error("Selector is not supposed to be on this index : " + selectorIndex);
    }
}

void BattleController::HandleAiActionSelection(AiActor& srcActor)
{
    const unsigned int moveIndex = rand() % m_currentActor->GetMoves().size(); // TODO : Move selection in BattleBehaviour (will use Random)
    const MoveDefinition md = m_currentActor->GetMove(moveIndex);
    m_currentCommand = CreateCommand(m_currentActor, md);

    const BattleBehaviour& srcBehaviour = srcActor.GetBehaviour();
    const LifeState lf = m_currentCommand.targetLifeState;
    m_targetActor = srcBehaviour.SelectTarget(FilterActorsByLifeState(GetActorsInTeam(m_currentCommand.targetTeam), lf));
    
    m_turnState = TurnState::HandleCommand;
}

void BattleController::HandleMoveSelection(const int moveIndex)
{
    CloseMoveSelection();
    
    const MoveDefinition md = m_currentActor->GetMove(moveIndex);
    m_currentCommand = CreateCommand(m_currentActor, md);

    OpenActorSelection(); // Selectable actors are filtered with targetTeam and targetLifeState in m_currentCommand
    m_turnState = TurnState::ActorSelection; // Should be in OpenActorSelection() ?
}

void BattleController::HandleCurrentCommand()
{
    SoundController::GetInstance().RequestChunk(m_currentCommand.sfx); // Request here, thus if ApplyDamage make a new request, the move sfx will not be played

    // Should call CreateSpriteAnimation() here, with animationPath and targetElement (instead of using setters) ?
    UiSpriteAnimation* moveAnimation = dynamic_cast<UiSpriteAnimation*>(m_uiComponentController.GetComponent("moveAnimation"));
    moveAnimation->SetAnimationPath(m_currentCommand.animation);
    moveAnimation->SetTargetElement(m_targetActor->GetSpritePath().id);
    moveAnimation->Open();

    switch (m_currentCommand.commandType) {
        case CommandType::Attack : {
            ApplyDamage(*m_currentActor, *m_targetActor);
            break;
        }
        case CommandType::Heal : {
            ApplyHeal(*m_currentActor, *m_targetActor);
            break;
        }
        default : 
            throw std::runtime_error("Unknown CommandType value"); 
    }
    m_turnState = TurnState::WaitingForAnimation;
}

void BattleController::InitializeActors(const std::string& battleFile)
{
    UiDynamicList* allyList = m_uiComponentController.CreateDynamicList("allyList", "battle_actor.uit");
    UiDynamicList* opponentList = m_uiComponentController.CreateDynamicList("opponentList", "battle_actor.uit");

    allyList->SetFirstItemParams(
        UiParams(m_uiController.GetResultFromPartialSize(PartialSize("background", Axis::Width, 0.2f)), Axis::Width, // Scale
        Anchor::RightIn, Anchor::TopIn, // Anchor
        m_uiController.GetResultFromPartialSize(PartialSize("background", Axis::Width, -0.2f)), // Padding
        m_uiController.GetResultFromPartialSize(PartialSize("background", Axis::Height, 0.05f))));
    opponentList->SetFirstItemParams(
        UiParams(m_uiController.GetResultFromPartialSize(PartialSize("background", Axis::Width, 0.2f)), Axis::Width, // Scale
        Anchor::LeftIn, Anchor::TopIn, // Anchor
        m_uiController.GetResultFromPartialSize(PartialSize("background", Axis::Width, 0.2f)), // Padding
        m_uiController.GetResultFromPartialSize(PartialSize("background", Axis::Height, 0.05f))));

    const std::unordered_map<unsigned int, MoveDefinition> moves = m_fileReader.ReadMoveFile("move_list");

    std::vector<DataBattleActor> dataActors = m_fileReader.ReadBattleFile(battleFile);
    unsigned int countAlly = 0;
    unsigned int countOpponent = 0;
    for (const DataBattleActor& data : dataActors) {
        std::unique_ptr<BattleActor> actor;
        
        std::string suffixKey;
        if (data.team == Team::Ally) {
            suffixKey = std::to_string(countAlly++);
            suffixKey += "_" + std::to_string(0);
        } else if (data.team == Team::Opponent) { // else ? (if I keep only Ally/Opponent)
            suffixKey = std::to_string(countOpponent++);
            suffixKey += "_" + std::to_string(1);
        }
        
        if (data.isAiActor)
            actor = std::make_unique<AiActor>(data.team, PrefixName+suffixKey, PrefixHealth+suffixKey, PrefixSprite+suffixKey, data.name, data.health, data.turnSpeed); // Later, AiActor will have more parameters
        else
            actor = std::make_unique<BattleActor>(data.team, PrefixName+suffixKey, PrefixHealth+suffixKey, PrefixSprite+suffixKey, data.name, data.health, data.turnSpeed);

        actor->SetSpritePath(data.spritePath); // Sprite path should be in BattleActor constructor ?
        
        std::vector<MoveDefinition> currentMoves;
        for (const unsigned int id : data.moveIds) {
            const MoveDefinition m = moves.at(id);
            SoundController::GetInstance().LoadChunk(m.sfxPath);
            currentMoves.push_back(m);
        }
        actor->SetMoves(currentMoves);
        
        actor->ComputeNextTurnTime(m_currentTime);
        m_turns.push(actor.get());
        
        if (data.team == Team::Ally) m_allies.push_back(actor.get());
        else if (data.team == Team::Opponent) m_opponents.push_back(actor.get()); // else ? (if I keep only Ally/Opponent)
        
        m_actors.push_back(std::move(actor));
    }

    allyList->SetNrItem(countAlly);
    allyList->Open();
    opponentList->SetNrItem(countOpponent);
    opponentList->Open();

    for (const std::unique_ptr<BattleActor>& b : m_actors) {
        m_uiController.UpdateText(b->GetName());
        m_uiController.UpdateText(b->GetHealth()); 
        m_uiController.UpdatePath(b->GetSpritePath());
    }
}

void BattleController::PlayNextTurn()
{
    switch (m_turnState) {
        case TurnState::Init : {
            m_currentActor = PopNextTurn();
            if (dynamic_cast<AiActor*>(m_currentActor) == nullptr) OpenActionSelection();
            m_turnState = TurnState::ActionSelection; 
        }
        
        case TurnState::ActionSelection : { // Should have TurnState::AiActionSelection to avoid dynamic_cast ?
            AiActor* aiActor = dynamic_cast<AiActor*>(m_currentActor);
            UiSelector* selector = dynamic_cast<UiSelector*>(m_uiComponentController.GetComponent("selector"));
            if (aiActor != nullptr) {
                HandleAiActionSelection(*aiActor);
            } else {
                if (selector->VerticalNavigation(m_eventState.uiDirection, m_eventState.isAction))
                    HandleActionSelection(selector->GetOptionIndex());
                SoundController::GetInstance().PlayRequestedChunk();
            }
            break;
        }

        case TurnState::MoveSelection : {
            UiSelector* selector = dynamic_cast<UiSelector*>(m_uiComponentController.GetComponent("selector"));
            if (selector->VerticalNavigation(m_eventState.uiDirection, m_eventState.isAction))
                HandleMoveSelection(selector->GetOptionIndex());
            SoundController::GetInstance().PlayRequestedChunk();
            break;
        }

        case TurnState::ActorSelection : {
            m_targetActor = GetActorSelection();
            if (m_targetActor != nullptr) {
                CloseActorSelection();
                m_turnState = TurnState::HandleCommand;
                break; // Will not play BaseSfx::Accept at the same time as m_currentCommand.sfx
            }
            SoundController::GetInstance().PlayRequestedChunk();
            break;
        }

        case TurnState::HandleCommand : {
            HandleCurrentCommand();
            SoundController::GetInstance().PlayRequestedChunk();
            break;
        }
        
        case TurnState::WaitingForAnimation : {
            UiSpriteAnimation* moveAnimation = dynamic_cast<UiSpriteAnimation*>(m_uiComponentController.GetComponent("moveAnimation"));
            if (moveAnimation->IsDone()) {
                moveAnimation->Close();
                m_turnState = TurnState::WaitingForText;
                break;
            }
            moveAnimation->ContinueAnimation();
            break;
        }

        case TurnState::WaitingForText : {
            if (m_eventState.isAction) {
                SoundController::GetInstance().RequestChunk(BaseSfx::Next);
                UiFrameText* frameText = dynamic_cast<UiFrameText*>(m_uiComponentController.GetComponent("frameText"));
                if (!frameText->NextText()) {
                    frameText->Close();
                    m_turnState = TurnState::End;
                } else {
                    SoundController::GetInstance().PlayRequestedChunk();
                }
            }
            break;
        }

        case TurnState::End : {
            if (m_exitEvent != ExitEvent::None) {
                m_uiComponentController.GetComponent("allyList")->ResetInstanceCount(); // Should not be here ?

                for (const std::unique_ptr<BattleActor>& b : m_actors) {
                    const std::vector<MoveDefinition>& moves = b->GetMoves();
                    for (const MoveDefinition m : moves)
                        SoundController::GetInstance().DeleteChunk(m.sfxPath);
                }
                
                Notify(m_exitEvent);
            } else {
                m_exitEvent = CheckBattleEnd();
                if (m_exitEvent != ExitEvent::None) {
                    UiFrameText* frameText = dynamic_cast<UiFrameText*>(m_uiComponentController.GetComponent("frameText"));
                    frameText->Open();
                    if (m_exitEvent == ExitEvent::ExitWin)
                        frameText->AddText({"You have defeated all the opponents !", "You win !"});
                    else if (m_exitEvent == ExitEvent::ExitLost)
                        frameText->AddText({"You no longer have any living fighters !", "You lose !"});
                    frameText->NextText();
                    m_turnState = TurnState::WaitingForText;
                } else {   
                    m_turnState = TurnState::Init;
                } 
            }
            break;
        }

        default:
            throw std::runtime_error("Unknown Turn value");
    }
}
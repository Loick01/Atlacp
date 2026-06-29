#include "battle/battle_controller.hpp"

#include "ai_battle/battle_behaviour.hpp"
#include "battle/ai_actor.hpp"
#include "core/file.hpp"
#include "sound/sound.hpp"
#include "ui/ui_controller.hpp"

namespace { // These values must be the same as in the template file used for the UiElement associated to a BattleActor (in BattleController::InitializeActors())
    // Should be constexpr std::string_view ?
    const std::string prefixName = "actorName";
    const std::string prefixHealth = "actorHealth";
    const std::string prefixSprite = "actorSprite";
}

BattleController::BattleController(FileReader& fileReader, UiController& uiController):
    m_uiController(uiController), m_fileReader(fileReader), m_currentActor(nullptr), m_targetActor(nullptr),
    m_turnState(TurnState::Init), m_exitEvent(ExitEvent::None), m_currentTime(0.f),
    m_allyList(uiController, "../data/ui/template/battle_actor.uit"), m_opponentList(uiController, "../data/ui/template/battle_actor.uit"),
    m_staticList(uiController, "../data/ui/file/battle_action_selection.uif"), m_dynamicList(uiController, "../data/ui/template/move_text.uit"), 
    m_selector(uiController, "../data/ui/template/selector.uit"), m_textSeries(uiController, "../data/ui/file/single_text_frame.uif")
{}

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
    if (m_selector.VerticalNavigation(m_eventState.uiDirection, m_eventState.isAction)) {
        m_textSeries.Close();
        m_selector.Close();
        std::vector<BattleActor*> actors = FilterActorsByLifeState(GetActorsInTeam(m_currentCommand.targetTeam), m_currentCommand.targetLifeState);
        BattleActor* targetActor = actors[m_selector.GetOptionIndex()];
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
            return baseValue * srcActor->GetStrength(); // Need a new field in BattleActor
        default:
            throw std::runtime_error("Unknown MoveType value");
    }
}

BattleCommand BattleController::CreateCommand(const BattleActor* srcActor, const MoveDefinition& md) const
{
    const unsigned int amount = ComputeMoveValue(md.moveType, md.value, srcActor);
    return BattleCommand(md.commandType, srcActor->GetTeam(), LifeState::Alive, amount, md.sfxPath); // Will not be LifeState::Alive
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
    
    m_textSeries.Open();
    m_textSeries.AddText({srcActor.GetName().value + " attacks " + targetActor.GetName().value + " !",
                        targetActor.GetName().value + " lost " + std::to_string(damage) + " HP !"});
    
    if (targetActor.GetLifeState() == LifeState::Dead) {
        m_textSeries.AddText({targetActor.GetName().value + " fainted !"});
        targetActor.SetSpritePath("../assets/battle/gravestone.png");    
        m_uiController.UpdatePath(targetActor.GetSpritePath());
        SoundController::GetInstance().RequestChunk(BaseSfx::Death);
    }
    
    m_textSeries.NextText(); // Should not be here ?
}

void BattleController::ApplyHeal(BattleActor& srcActor, BattleActor& targetActor)
{
    const unsigned int hp = m_currentCommand.moveValue;
    targetActor.AddHealth(hp);
    m_uiController.UpdateText(targetActor.GetHealth());
    
    m_textSeries.Open();
    m_textSeries.AddText({srcActor.GetName().value + " gives a potion to " + targetActor.GetName().value + ".",
                          targetActor.GetName().value + " recovered " + std::to_string(hp) + " HP !"});

    m_textSeries.NextText();
}

void BattleController::OpenActionSelection()
{
    m_staticList.Open();
    m_selector.Open();
    m_selector.SetOptionKeys(m_staticList.GetItemsKey());
    // When selector file is build, scale is based on root element
    m_uiController.UpdateScalingSize(m_selector.GetKey(), PartialSize{m_staticList.GetKey(), Axis::Height, 1.f}); 
    m_uiController.UpdatePadding(m_selector.GetKey(), PartialSize(m_staticList.GetKey(), Axis::Width, -0.05f), Axis::Width);
}

void BattleController::CloseActionSelection()
{
    m_selector.Close();
    m_staticList.Close();
}

void BattleController::OpenMoveSelection()
{
    const std::vector<MoveDefinition>& moves = m_currentActor->GetMoves();
    m_dynamicList.SetFirstItemParams(
        UiParams(m_uiController.GetResultFromPartialSize(PartialSize("frame", Axis::Width, 0.5f)), Axis::Width, // Scale
        Anchor::LeftIn, Anchor::TopIn, // Anchor
        m_uiController.GetResultFromPartialSize(PartialSize("frame", Axis::Width, 0.06f)), // Padding
        m_uiController.GetResultFromPartialSize(PartialSize("frame", Axis::Height, 0.25f))));
    m_dynamicList.SetNrItem(moves.size());
    m_dynamicList.Open();
    
    const std::vector<UiKey> keys = m_dynamicList.GetItemsKey();
    for (unsigned int i = 0 ; i < keys.size() ; i++) // keys.size = moves.size()
        m_uiController.UpdateText(keys[i], moves[i].name);

    m_selector.Open();
    m_selector.SetOptionKeys(keys);
    m_uiController.UpdateScalingSize(m_selector.GetKey(), PartialSize{m_dynamicList.GetKey(), Axis::Height, 1.f});
    m_uiController.UpdatePadding(m_selector.GetKey(), PartialSize(m_dynamicList.GetKey(), Axis::Width, -0.05f), Axis::Width);
}

void BattleController::CloseMoveSelection()
{
    m_selector.Close();
    m_dynamicList.Close();
}

void BattleController::OpenSelectorOnActors()
{
    m_selector.Open();
    std::vector<BattleActor*> aliveActors = FilterActorsByLifeState(GetActorsInTeam(m_currentCommand.targetTeam), m_currentCommand.targetLifeState);
    std::vector<UiKey> keys;
    for (const BattleActor* actor : aliveActors)
        keys.push_back(actor->GetSpritePath().id); // SpritePath.id is the parent key of each item in m_allyList/m_opponentList
    m_selector.SetOptionKeys(keys);
    m_uiController.UpdateScalingSize(m_selector.GetKey(), PartialSize{m_currentActor->GetSpritePath().id, Axis::Height, 0.2f}); // m_currentActor ?
    m_uiController.UpdatePadding(m_selector.GetKey(), PartialSize(m_currentActor->GetSpritePath().id, Axis::Width, 0.2f), Axis::Width);

    m_textSeries.Open();
    m_textSeries.AddText({"Select a BattleActor"}); // TODO
    m_textSeries.NextText();
}

void BattleController::HandleActionSelection(const int selectorIndex)
{
    CloseActionSelection();
    
    switch (selectorIndex) {
        case 0: {
            OpenMoveSelection();
            m_turnState = TurnState::MoveSelection;
            break;
        }

        case 1:
            m_textSeries.Open();
            m_textSeries.AddText({"This turn has been skipped"});
            m_textSeries.NextText();
            m_turnState = TurnState::Waiting;
            break;
            
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

    OpenSelectorOnActors(); // Selectable actors are filtered with targetTeam and targetLifeState in m_currentCommand
    m_turnState = TurnState::ActorSelection;
}

void BattleController::HandleCurrentCommand()
{
    SoundController::GetInstance().RequestChunk(m_currentCommand.sfx); // Request here, thus if ApplyDamage make a new request, the move sfx will not be played
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
    m_turnState = TurnState::Waiting;
}

void BattleController::InitializeActors(const std::string& battleFile)
{
    m_allyList.SetFirstItemParams(
        UiParams(m_uiController.GetResultFromPartialSize(PartialSize("background", Axis::Width, 0.2f)), Axis::Width, // Scale
        Anchor::RightIn, Anchor::TopIn, // Anchor
        m_uiController.GetResultFromPartialSize(PartialSize("background", Axis::Width, -0.2f)), // Padding
        m_uiController.GetResultFromPartialSize(PartialSize("background", Axis::Height, 0.05f))));
    m_opponentList.SetFirstItemParams(
        UiParams(m_uiController.GetResultFromPartialSize(PartialSize("background", Axis::Width, 0.2f)), Axis::Width, // Scale
        Anchor::LeftIn, Anchor::TopIn, // Anchor
        m_uiController.GetResultFromPartialSize(PartialSize("background", Axis::Width, 0.2f)), // Padding
        m_uiController.GetResultFromPartialSize(PartialSize("background", Axis::Height, 0.05f))));

    const std::unordered_map<unsigned int, MoveDefinition> moves = m_fileReader.ReadMoveFile("../data/battle/moves/move_list");

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
            actor = std::make_unique<AiActor>(data.team, prefixName+suffixKey, prefixHealth+suffixKey, prefixSprite+suffixKey, data.name, data.health, data.turnSpeed); // Later, AiActor will have more parameters
        else
            actor = std::make_unique<BattleActor>(data.team, prefixName+suffixKey, prefixHealth+suffixKey, prefixSprite+suffixKey, data.name, data.health, data.turnSpeed);

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

    m_allyList.SetNrItem(countAlly);
    m_opponentList.SetNrItem(countOpponent);
    m_allyList.Open();
    m_opponentList.Open();

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
            if (aiActor != nullptr) {
                HandleAiActionSelection(*aiActor);
            } else {
                if (m_selector.VerticalNavigation(m_eventState.uiDirection, m_eventState.isAction))
                    HandleActionSelection(m_selector.GetOptionIndex());
                SoundController::GetInstance().PlayRequestedChunk();
            }
            break;
        }

        case TurnState::MoveSelection : {
            if (m_selector.VerticalNavigation(m_eventState.uiDirection, m_eventState.isAction))
                HandleMoveSelection(m_selector.GetOptionIndex());
            SoundController::GetInstance().PlayRequestedChunk();
            break;
        }

        case TurnState::ActorSelection : {
            m_targetActor = GetActorSelection();
            if (m_targetActor != nullptr) {
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
        
        case TurnState::Waiting : {
            if (m_eventState.isAction) {
                SoundController::GetInstance().RequestChunk(BaseSfx::Next);
                if (!m_textSeries.NextText()) {
                    m_textSeries.Close();
                    m_turnState = TurnState::End;
                } else {
                    SoundController::GetInstance().PlayRequestedChunk();
                }
            }
            break;
        }

        case TurnState::End : {
            if (m_exitEvent != ExitEvent::None) {
                m_allyList.ResetInstanceCount(); // Should not be here ?
                Notify(m_exitEvent);
            } else {
                m_exitEvent = CheckBattleEnd();
                if (m_exitEvent != ExitEvent::None) {
                    m_textSeries.Open();
                    if (m_exitEvent == ExitEvent::ExitWin)
                        m_textSeries.AddText({"You have defeated all the opponents !", "You win !"});
                    else if (m_exitEvent == ExitEvent::ExitLost)
                        m_textSeries.AddText({"You no longer have any living fighters !", "You lose !"});
                    m_textSeries.NextText();
                    m_turnState = TurnState::Waiting;
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
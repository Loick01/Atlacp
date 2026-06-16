#include "battle/battle_controller.hpp"

#include "ai_battle/battle_behaviour.hpp"
#include "battle/ai_actor.hpp"
#include "core/file.hpp"
#include "ui/ui_controller.hpp"

namespace { // These values must be the same as in the template file used for the UiElement associated to a BattleActor (in BattleController::InitializeActors())
    const std::string prefixName = "actorName";
    const std::string prefixHealth = "actorHealth";
    const std::string prefixSprite = "actorSprite";
}

BattleController::BattleController(FileReader& fileReader, UiController& uiController):
    m_uiController(uiController), m_fileReader(fileReader), m_currentActor(nullptr), 
    m_turnState(TurnState::Init), m_exitEvent(ExitEvent::None), m_currentTime(0.f),
    m_allyList(uiController, "../data/ui/template/battle_actor.uit"), m_opponentList(uiController, "../data/ui/template/battle_actor.uit"),
    m_actionList(uiController, "../data/ui/file/action_selection.uif"), m_selector(uiController, "../data/ui/template/selector.uit"),
    m_textSeries(uiController, "../data/ui/file/single_text_frame.uif")
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
    if (m_eventState.uiDirection == Direction::Down) {
        m_selector.Next();
    } else if (m_eventState.uiDirection == Direction::Up) {
        m_selector.Previous();
    } else if (m_eventState.isAction) {
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
    return BattleCommand(md.commandType, srcActor->GetTeam(), LifeState::Alive, amount); // Will not be LifeState::Alive
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
    m_actionList.Open();
    m_selector.Open();
    m_selector.SetOptionKeys(m_actionList.GetItemsKey());
    // When selector file is build, scale is based on root element
    m_uiController.UpdateScalingSize(m_selector.GetKey(), PartialSize{m_actionList.GetKey(), Axis::Height, 0.8f}); 
}

void BattleController::CloseActionSelection()
{
    m_selector.Close();
    m_actionList.Close();
}

void BattleController::SetSelectorOptions(const Team team)
{
    std::vector<BattleActor*> aliveActors = FilterActorsByLifeState(GetActorsInTeam(team), LifeState::Alive); // LifeState::Alive ?
    std::vector<UiKey> keys;
    for (const BattleActor* actor : aliveActors)
        keys.push_back(actor->GetSpritePath().id); // SpritePath.id is the parent key of each item in m_allyList/m_opponentList
    m_selector.SetOptionKeys(keys); // Should not be here ?
}

void BattleController::HandleActionSelection(const int selectorIndex)
{
    switch (selectorIndex) {
        case 0: {
            CloseActionSelection();
            m_textSeries.Open();
            m_textSeries.AddText({"Choose an opponent to attack"});
            m_textSeries.NextText();
            
            const unsigned int moveIndex = 0; // TODO : Move selection
            const MoveDefinition md = (m_fileReader.ReadMoveFile("../data/battle/moves/move_list"))[moveIndex]; // Will be done only once
            m_currentCommand = CreateCommand(m_currentActor, md);
            
            m_selector.Open();
            m_uiController.UpdateScalingSize(m_selector.GetKey(), PartialSize{m_currentActor->GetSpritePath().id, Axis::Height, 0.2f}); // m_currentActor ?
            SetSelectorOptions(m_currentCommand.targetTeam);
            
            m_turnState = TurnState::ActorSelection;
            break;
        }
        
        case 1: {
            CloseActionSelection();
            m_textSeries.Open();
            m_textSeries.AddText({"Choose an ally to heal"});
            m_textSeries.NextText();
            
            const unsigned int moveIndex = 2; // TODO : Move selection
            const MoveDefinition md = (m_fileReader.ReadMoveFile("../data/battle/moves/move_list"))[moveIndex]; // Will be done only once
            m_currentCommand = CreateCommand(m_currentActor, md);

            m_selector.Open();
            m_uiController.UpdateScalingSize(m_selector.GetKey(), PartialSize{m_currentActor->GetSpritePath().id, Axis::Height, 0.2f}); // m_currentActor ?
            SetSelectorOptions(m_currentCommand.targetTeam);

            m_turnState = TurnState::ActorSelection;
            break;
        }

        case 2:
            CloseActionSelection();
            m_turnState = TurnState::End;
            break;
            
        case 3: {
            Notify(ExitEvent::ExitWin); // Not ExitWin
            break;
        }

        default:
            throw std::runtime_error("Selector is not supposed to be on this index : " + selectorIndex);
    }
}

void BattleController::HandleAiActionSelection(AiActor& srcActor)
{
    const unsigned int moveIndex = 0; // TODO : Move selection
    const MoveDefinition md = (m_fileReader.ReadMoveFile("../data/battle/moves/move_list"))[moveIndex]; // Will be done only once
    m_currentCommand = CreateCommand(m_currentActor, md);

    const BattleBehaviour& srcBehaviour = srcActor.GetBehaviour();
    const LifeState lf = m_currentCommand.targetLifeState;
    BattleActor* targetActor = srcBehaviour.SelectTarget(FilterActorsByLifeState(GetActorsInTeam(m_currentCommand.targetTeam), lf));
    HandleCurrentCommand(targetActor);
    m_turnState = TurnState::Waiting; // Should be in HandleCurrentCommand() ?
}

void BattleController::HandleCurrentCommand(BattleActor* targetActor)
{
    switch (m_currentCommand.commandType) {
        case CommandType::Attack : {
            ApplyDamage(*m_currentActor, *targetActor);
            break;
        }
        case CommandType::Heal : {
            ApplyHeal(*m_currentActor, *targetActor);
            break;
        }
        default : 
            throw std::runtime_error("Unknown CommandType value"); 
    }
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
        
        case TurnState::ActionSelection : {
            AiActor* aiActor = dynamic_cast<AiActor*>(m_currentActor); // Already a dynamic_cast in TurnState::Init ?
            if (aiActor != nullptr) {
                HandleAiActionSelection(*aiActor);
            } else {
                if (m_eventState.uiDirection == Direction::Down)
                    m_selector.Next();
                else if (m_eventState.uiDirection == Direction::Up)
                    m_selector.Previous();
                else if (m_eventState.isAction)
                    HandleActionSelection(m_selector.GetOptionIndex());
            }
            break;
        }

        case TurnState::ActorSelection : {
            BattleActor* targetActor = GetActorSelection();
            if (targetActor != nullptr) {
                HandleCurrentCommand(targetActor);
                m_turnState = TurnState::Waiting; // Should be in HandleCurrentCommand() ? (for every case ?)
            }
            break;
        }

        case TurnState::Waiting : {
            if (m_eventState.isAction) {
                if (!m_textSeries.NextText()) {
                    m_textSeries.Close();
                    m_turnState = TurnState::End; // ?
                }
            } 
            break;
        }

        case TurnState::End : {
            if (m_exitEvent != ExitEvent::None) {
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
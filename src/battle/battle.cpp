#include "battle/battle.hpp"

BattleActor::BattleActor(const Team team, const ElementKey& nameId, const ElementKey& healthId, const std::string name, const unsigned int health):
    m_team(team), m_name(nameId, name), m_health(healthId, health), m_strength(10), m_turnSpeed(5) // m_turnSpeed will not be here 
{}

UiValue<std::string> BattleActor::GetName() const
{
    return m_name;
}

UiValue<unsigned int> BattleActor::GetHealth() const
{
    return m_health;
}

unsigned int BattleActor::GetStrength() const
{
    return m_strength;
}

Team BattleActor::GetTeam() const
{
    return m_team;
}

void BattleActor::ModifyHealth(const int hp)
{
    m_health.value += hp;
}

BattleController::BattleController(UiController& uiController):
    m_uiController(uiController), m_currentTurn(Turn::Init), m_currentActor(nullptr),
    m_allyList(uiController), m_opponentList(uiController), m_frameList(uiController),
    m_selector(uiController, "../data/ui/selector_template"), m_textSeries(uiController, "../data/ui/single_text_frame_template")
{
    // Will not be here
    m_actors.push_back(BattleActor(Team::Ally, "actorAName", "actorAHealth", "Howler", 100));
    m_actors.push_back(BattleActor(Team::Opponent, "actorBName", "actorBHealth", "Bone Appetit", 100));
    m_turns.push(&(m_actors[0])); // Remove
    m_turns.push(&(m_actors[1])); // Remove
}

void BattleController::UpdateStatus()
{
    for (unsigned int i = 0 ; i < m_actors.size() ; i++) {
        m_uiController.UpdateText(m_actors[i].GetName());
        m_uiController.UpdateText(m_actors[i].GetHealth()); 
    }
}

bool BattleController::HasAliveActor(const Team team)
{
    for (unsigned int i = 0 ; i < m_actors.size() ; i++) {
        const BattleActor& actor = m_actors[i];
        if (actor.GetTeam() == team) {
            if (actor.GetHealth().value > 0) {
                return true;
            }
        }
    }
    return false;
}

void BattleController::CheckActorHealth()
{
    if (!HasAliveActor(Team::Ally))
        Notify(ExitEvent::ExitLost);
    else if (!HasAliveActor(Team::Opponent))
        Notify(ExitEvent::ExitWin);
}

void BattleController::OpenAllyMoveSelection()
{
    m_frameList.SetFilepath("../data/ui/ally_move_selection");
    m_frameList.Open();
    m_selector.Open();
    m_selector.SetParents(m_frameList.GetElementsKey()); // Could be call before Open() ?
    // When selector file is build, scale is based on root element
    m_uiController.UpdateScalingSize(m_selector.GetKey(), PartialSize{"option0", Axis::Height, 0.8f}); 
}

void BattleController::CloseAllyMoveSelection()
{
    m_selector.Close();
    m_frameList.Close();
}

unsigned int BattleController::TakeDamage(BattleActor& source, BattleActor& target)
{
    const unsigned int damage = source.GetStrength();
    target.ModifyHealth(-damage);
    CheckActorHealth();
    m_uiController.UpdateText(target.GetHealth());
    return damage;
}

unsigned int BattleController::TakeHealth(BattleActor& source) // Will use Object from inventory
{
    const unsigned int hp = source.GetStrength(); // For now, I use the source strength
    source.ModifyHealth(hp); // clamp
    m_uiController.UpdateText(source.GetHealth());
    return hp;
}

void BattleController::HandleOptionSelection(BattleActor& srcActor, const int selectorIndex)
{
    switch (selectorIndex) {
        case 0: {
            CloseAllyMoveSelection();
            m_textSeries.Open();
            m_textSeries.AddText({"Choose an opponent to attack"});
            m_selector.Open();
            m_selector.SetParents({"actorASprite", "actorBSprite"}); // Remove
            // m_selector.SetParents(m_opponentList.GetElementsKey());
            m_uiController.UpdateScalingSize(m_selector.GetKey(), PartialSize{"actorASprite", Axis::Height, 0.2f}); // Not "actorASprite"
            m_currentTurn = Turn::ActorSelection;
            break;
        }
        
        case 1: {
            CloseAllyMoveSelection();
            const unsigned int hp = TakeHealth(srcActor);
            m_currentTurn = Turn::Waiting;
            m_textSeries.Open();
            m_textSeries.AddText({srcActor.GetName().value + " drinks a potion",
                               srcActor.GetName().value + " recovered " + std::to_string(hp) + " HP !"});

            break;
        }
        case 2:
            break;
        case 3: {
            Notify(ExitEvent::ExitWin); // Not ExitWin
            break;
        }

        default:
            throw std::runtime_error("Selector is not supposed to be on this index : " + selectorIndex);
    }
}

void BattleController::HandleActorSelection()
{
    if (m_eventState.uiDirection == Direction::Right) {
        m_selector.Next();
    } else if (m_eventState.uiDirection == Direction::Left) {
        m_selector.Previous();
    } else if (m_eventState.isAction) {
        // m_textSeries.Close(); // Not really necessary because Open() call in ApplyDamage will not build the associated file if it is still open
        ApplyDamage(*m_currentActor, m_selector.GetIndex());
        m_selector.Close();
        m_currentTurn = Turn::Waiting;
    }
}

void BattleController::ApplyDamage(BattleActor& srcActor, const int selectorIndex)
{
    BattleActor& targetActor = m_actors[selectorIndex]; // For now, the selector can choose any actors (Ally or Opponent). Later I will filter them
    const unsigned int damage = TakeDamage(srcActor, targetActor); 
    m_textSeries.Open();
    m_textSeries.AddText({srcActor.GetName().value + " attacks " + targetActor.GetName().value + " !",
                        targetActor.GetName().value + " lost " + std::to_string(damage) + " HP !"});
}

void BattleController::HandleOpponentTurn(BattleActor& srcActor)
{
    BattleActor& targetActor = m_actors[0]; // TODO : Behaviour
    const unsigned int damage = TakeDamage(srcActor, targetActor); 
    m_currentTurn = Turn::Waiting;
    m_textSeries.Open();
    m_textSeries.AddText({srcActor.GetName().value + " attacks " + targetActor.GetName().value + " !",
                        targetActor.GetName().value + " lost " + std::to_string(damage) + " HP !"});
}

BattleActor* BattleController::GetNextTurn()
{
    // TODO
    BattleActor* actor = m_turns.front();  // Not const ?
    m_turns.pop();
    m_turns.push(actor);
    return actor;
}

void BattleController::PlayNextTurn()
{
    switch (m_currentTurn) {
        case Turn::Init : {
            m_currentActor = GetNextTurn();
            if (m_currentActor->GetTeam() == Team::Ally) OpenAllyMoveSelection();
            m_currentTurn = Turn::OptionSelection; 
        }
        
        case Turn::OptionSelection : {
            switch (m_currentActor->GetTeam()) {
                case Team::Ally : {
                    if (m_eventState.uiDirection == Direction::Down) {
                        m_selector.Next();
                    } else if (m_eventState.uiDirection == Direction::Up) {
                        m_selector.Previous();
                    } else if (m_eventState.isAction) {
                        HandleOptionSelection(*m_currentActor, m_selector.GetIndex());
                    }   
                    break;
                }
                case Team::Opponent : {
                    HandleOpponentTurn(*m_currentActor);
                    break;
                }
                default:
                    throw std::runtime_error("Unknown Team value");
            }
            break;
        }

        case Turn::ActorSelection : {
            HandleActorSelection();
            break;
        }

        case Turn::Waiting : {
            if (m_eventState.isAction) {
                if (!m_textSeries.Next()) {
                    m_textSeries.Close();
                    m_currentTurn = Turn::Init;
                }
            } 
            break;
        }

        default:
            throw std::runtime_error("Unknown Turn value");
    }
}
#include "battle.hpp"

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
    m_selector(uiController, "../data/ui/selector_template"), m_textList(uiController, "../data/ui/single_text_frame_template")
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

void BattleController::OpenPlayerOption()
{
    m_uiController.BuildUiFile("../data/ui/battle_player_option_template"); // Will call Open() on option list
    m_selector.Open();
    m_selector.SetParents({"option0", "option1", "option2", "option3"}); // Will be get from option
}

void BattleController::ClosePlayerOption()
{
    m_selector.Close();
    m_uiController.DeleteElement("option0"); // Will call Close() on option list 
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

void BattleController::HandlePlayerSelection(BattleActor& srcActor, const int selectorIndex)
{
    switch (selectorIndex) {
        case 0: {
            BattleActor& targetActor = m_actors[1]; // TODO : Target selection
            const unsigned int damage = TakeDamage(srcActor, targetActor); 
            ClosePlayerOption();
            m_currentTurn = Turn::Waiting;
            m_textList.Open();
            m_textList.AddText({srcActor.GetName().value + " attacks !",
                               targetActor.GetName().value + " lost " + std::to_string(damage) + " HP !"});
            
            break;
        }
        
        case 1: {
            const unsigned int hp = TakeHealth(srcActor);
            ClosePlayerOption();
            m_currentTurn = Turn::Waiting;
            m_textList.Open();
            m_textList.AddText({srcActor.GetName().value + " drinks a potion",
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

void BattleController::HandleOpponentTurn(BattleActor& srcActor)
{
    BattleActor& targetActor = m_actors[0]; // TODO : Target selection
    const unsigned int damage = TakeDamage(srcActor, targetActor); 
    m_currentTurn = Turn::Waiting;
    m_textList.Open();
    m_textList.AddText({srcActor.GetName().value + " attacks !",
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
            if (m_currentActor->GetTeam() == Team::Ally) OpenPlayerOption();
            m_currentTurn = Turn::Playing; 
        }
        
        case Turn::Playing : {
            switch (m_currentActor->GetTeam()) {
                case Team::Ally : {
                    if (m_eventState.uiDirection == Direction::Down) {
                        m_selector.Next();
                    } else if (m_eventState.uiDirection == Direction::Up) {
                        m_selector.Previous();
                    } else if (m_eventState.isAction) {
                        HandlePlayerSelection(*m_currentActor, m_selector.GetIndex());
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

        case Turn::Waiting : {
            if (m_eventState.isAction) {
                if (!m_textList.Next()) {
                    m_textList.Close();
                    m_currentTurn = Turn::Init;
                }
            } 
            break;
        }

        default:
            throw std::runtime_error("Unknown Turn value");
    }
}
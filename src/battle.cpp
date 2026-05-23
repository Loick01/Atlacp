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
    m_uiController(uiController), m_currentTurn(Turn::Playing),
    m_selector(uiController), m_textList(uiController, "../data/ui/single_text_frame_template")
{
    // Will not be here 
    m_actors.push_back(BattleActor(Team::Ally, "actorAName", "actorAHealth", "Howler", 100));
    m_actors.push_back(BattleActor(Team::Opponent, "actorBName", "actorBHealth", "Bone Appetit", 100));
    m_nextActor = GetNextTurn();
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
    m_uiController.BuildUiFile("../data/ui/battle_player_option_template");
    m_selector.Reset(); 
}

void BattleController::ClosePlayerOption()
{
    m_uiController.DeleteElement("option0");
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

void BattleController::HandlePlayerSelection(const int index)
{
    switch (index) {
        case 0: {
            const unsigned int damage = TakeDamage(m_actors[0], m_actors[1]); 
            ClosePlayerOption();
            m_currentTurn = Turn::Waiting;
            m_textList.Open();
            m_textList.AddText({m_actors[0].GetName().value + " attacks !",
                               m_actors[1].GetName().value + " lost " + std::to_string(damage) + " HP !"});
            
            break;
        }
        
        case 1: {
            const unsigned int hp = TakeHealth(m_actors[0]);
            ClosePlayerOption();
            m_currentTurn = Turn::Waiting;
            m_textList.Open();
            m_textList.AddText({m_actors[0].GetName().value + " drinks a potion",
                               m_actors[0].GetName().value + " recovered " + std::to_string(hp) + " HP !"});

            break;
        }
        case 2:
            break;
        case 3: {
            Notify(ExitEvent::ExitWin); // Not ExitWin
            break;
        }

        default:
            throw std::runtime_error("Selector is not supposed to be on this index : " + index);
    }
}

void BattleController::HandleEnemyTurn()
{
    const unsigned int damage = TakeDamage(m_actors[1], m_actors[0]); 
    m_currentTurn = Turn::Waiting;
    m_textList.Open();
    m_textList.AddText({m_actors[1].GetName().value + " attacks !",
                        m_actors[0].GetName().value + " lost " + std::to_string(damage) + " HP !"});
}

BattleActor* BattleController::GetNextTurn()
{
    return &(m_actors[0]); // TODO
}

void BattleController::PlayNextTurn()
{
    switch (m_currentTurn) {
        case Turn::Playing : {
            // const BattleActor* actor = m_turns.front();  // Not const ?
            // m_turns.pop();

            switch (m_nextActor->GetTeam()) {
                case Team::Ally : {
                    if (m_eventState.uiDirection == Direction::Down) {
                        m_selector.Next();
                    } else if (m_eventState.uiDirection == Direction::Up) {
                        m_selector.Previous();
                    } else if (m_eventState.isAction) {
                        HandlePlayerSelection(m_selector.GetIndex());
                    }   
                    break;
                }
                case Team::Opponent : {
                    HandleEnemyTurn();
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
                    m_currentTurn = Turn::Playing;
                    m_nextActor = GetNextTurn();
                    if (m_nextActor->GetTeam() == Team::Ally) OpenPlayerOption(); // Should not be here ? 
                    m_textList.Close();
                }
            } 
            break;
        }

        default:
            break;
    }
}
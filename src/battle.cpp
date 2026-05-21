#include "battle.hpp"

BattleActor::BattleActor(const ElementKey& nameId, const ElementKey& healthId, const std::string name, const unsigned int health):
    m_name(nameId, name), m_health(healthId, health), m_strength(10)
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

void BattleActor::ModifyHealth(const int hp)
{
    m_health.value += hp;
}

BattleController::BattleController(UiController& uiController, const BattleActor actorA, const BattleActor actorB):
    m_uiController(uiController), m_actorA(actorA), m_actorB(actorB), m_currentTurn(Turn::ActorA),
    m_selector(uiController), m_textList(uiController, "../data/ui/single_text_frame_template")
{}

void BattleController::UpdateStatus()
{
    m_uiController.UpdateText(m_actorA.GetName()); 
    m_uiController.UpdateText(m_actorB.GetName());
    m_uiController.UpdateText(m_actorA.GetHealth()); 
    m_uiController.UpdateText(m_actorB.GetHealth());
}

void BattleController::CheckActorHealth()
{
    if (m_actorA.GetHealth().value <= 0)
        Notify(ExitEvent::ExitLost);
    else if (m_actorB.GetHealth().value <= 0)
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
            const unsigned int damage = TakeDamage(m_actorA, m_actorB); 
            ClosePlayerOption();
            m_currentTurn = Turn::WaitingA;
            m_textList.Open();
            m_textList.AddText({m_actorA.GetName().value + " attacks !",
                               m_actorB.GetName().value + " lost " + std::to_string(damage) + " HP !"});
            
            break;
        }
        
        case 1: {
            const unsigned int hp = TakeHealth(m_actorA);
            ClosePlayerOption();
            m_currentTurn = Turn::WaitingA;
            m_textList.Open();
            m_textList.AddText({m_actorA.GetName().value + " drinks a potion",
                               m_actorA.GetName().value + " recovered " + std::to_string(hp) + " HP !"});

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
    const unsigned int damage = TakeDamage(m_actorB, m_actorA); 
    m_currentTurn = Turn::WaitingB;
    m_textList.Open();
    m_textList.AddText({m_actorB.GetName().value + " attacks !",
                        m_actorA.GetName().value + " lost " + std::to_string(damage) + " HP !"});
}

void BattleController::PlayFight()
{
    switch(m_currentTurn) {
        case Turn::ActorA : {
            if (m_eventState.uiDirection == Direction::Down) {
                m_selector.Next();
            } else if (m_eventState.uiDirection == Direction::Up) {
                m_selector.Previous();
            } else if (m_eventState.isAction) {
                HandlePlayerSelection(m_selector.GetIndex());
            }   
            break;
        }

        case Turn::ActorB : {
            HandleEnemyTurn();
            break;
        }

        case Turn::WaitingA : {
            if (m_eventState.isAction) { // Try to merge with case Turn::WaitingB ?
                if (!m_textList.Next()) {
                    m_currentTurn = Turn::ActorB;
                    m_textList.Close();
                }
            } 
            break;
        }

        case Turn::WaitingB : {
            if (m_eventState.isAction) {
                if (!m_textList.Next()) {
                    m_currentTurn = Turn::ActorA;
                    m_textList.Close();
                    OpenPlayerOption();
                }
            }
            break;
        }
    }
}
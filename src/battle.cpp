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

void BattleActor::RemoveHealth(const unsigned int damage)
{
    m_health.value -= damage;
}

BattleController::BattleController(UiController& uiController, const BattleActor actorA, const BattleActor actorB):
    m_uiController(uiController), m_actorA(actorA), m_actorB(actorB), m_currentTurn(Turn::ActorA), m_selector(uiController)
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

void BattleController::InitPlayerTurn()
{
    // Will not be here
    m_uiController.BuildUiFile("../data/ui/battle_player_option_template");
    m_selector.Reset(); 
}

void BattleController::PlayTurn(BattleActor& source, BattleActor& target)
{
    target.RemoveHealth(source.GetStrength());
    CheckActorHealth();
    m_uiController.UpdateText(target.GetHealth());
}

void BattleController::PlayFight()
{
    switch(m_currentTurn) {
        case Turn::ActorA : {
            if (m_eventState.uiDirection == Direction::Down) {
                m_selector.Next();
            } else if (m_eventState.uiDirection == Direction::Up) {
                m_selector.Previous();
            } else if (m_eventState.isAction) { // TODO
                PlayTurn(m_actorA, m_actorB);
                // Will not be here ?
                m_uiController.DeleteElement("frame");
                m_currentTurn = Turn::ActorB; // SwitchTurn function (could be in PlayTurn) ?
            }   
            break;
        }
        case Turn::ActorB : {
            if (m_eventState.isAction) { // TODO
                PlayTurn(m_actorB, m_actorA);
                // Will not be here ?
                // m_uiController.UpdateText("mainText", "Turn A");
                InitPlayerTurn();
                m_currentTurn = Turn::ActorA; // SwitchTurn function (could be in PlayTurn) ?
            }
            break;
        }
    }
}
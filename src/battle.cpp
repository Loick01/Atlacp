#include "battle.hpp"

BattleActor::BattleActor(const ElementKey& nameId, const ElementKey& healthId, const std::string name, const unsigned int health):
    m_name(nameId, name), m_health(healthId, health), m_strength(10)
{}

ElementKey BattleActor::GetHealthId() const
{
    return m_health.id;
}

ElementKey BattleActor::GetNameId() const
{
    return m_name.id;
}

std::string BattleActor::GetName() const
{
    return m_name.value;
}

unsigned int BattleActor::GetHealth() const
{
    return m_health.value;
}

unsigned int BattleActor::GetStrength() const
{
    return m_strength;
}

void BattleActor::RemoveHealth(const unsigned int damage)
{
    m_health.value -= damage;
}

BattleController::BattleController(const BattleActor actorA, const BattleActor actorB):
    m_uiController(nullptr), m_actorA(actorA), m_actorB(actorB), m_currentTurn(Turn::ActorA), m_selector()
{}

void BattleController::SetUiController(UiController* uiController)
{
    m_uiController = uiController;
    m_selector.SetUiController(uiController);
}

void BattleController::UpdateStatus()
{
    m_uiController->UpdateText(m_actorA.GetNameId(), m_actorA.GetName()); 
    m_uiController->UpdateText(m_actorB.GetNameId(), m_actorB.GetName());
    m_uiController->UpdateText(m_actorA.GetHealthId(), std::to_string(m_actorA.GetHealth())); 
    m_uiController->UpdateText(m_actorB.GetHealthId(), std::to_string(m_actorB.GetHealth()));
}

void BattleController::CheckActorHealth()
{
    if (m_actorA.GetHealth() <= 0)
        Notify(ExitEvent::ExitLost);
    else if (m_actorB.GetHealth() <= 0)
        Notify(ExitEvent::ExitWin);
}

void BattleController::InitPlayerTurn()
{
    // Will not be here
    m_uiController->BuildUiFile("../data/ui/option_template");
    m_uiController->BuildUiFile("../data/ui/selector"); 
    m_uiController->GetElement("mainBox")->UpdatePosition();
}

void BattleController::PlayTurn(BattleActor& source, BattleActor& target)
{
    target.RemoveHealth(source.GetStrength());
    CheckActorHealth();
    m_uiController->UpdateText(target.GetHealthId(), std::to_string(target.GetHealth()));  // I will add UiController::UpdateText(UiValue)
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
                m_uiController->DeleteElement("option0");
                m_currentTurn = Turn::ActorB; // SwitchTurn function (could be in PlayTurn) ?
            }   
            break;
        }
        case Turn::ActorB : {
            if (m_eventState.isAction) { // TODO
                PlayTurn(m_actorB, m_actorA);
                // Will not be here ?
                // m_uiController->UpdateText("mainText", "Turn A");
                InitPlayerTurn();
                m_currentTurn = Turn::ActorA; // SwitchTurn function (could be in PlayTurn) ?
            }
            break;
        }
    }
}
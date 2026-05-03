#include "battle.hpp"

BattleActor::BattleActor(const std::string name, const unsigned int health):
    m_name(name), m_health(health)
{

}

std::string BattleActor::GetName() const
{
    return m_name;
}

unsigned int BattleActor::GetHealth() const
{
    return m_health;
}

unsigned int BattleActor::GetStrength() const
{
    return m_strength;
}

void BattleActor::TakeDamage(const unsigned int strength)
{
    m_health -= strength;
}

BattleController::BattleController(const BattleActor actorA, const BattleActor actorB):
    m_actorA(actorA), m_actorB(actorB), m_currentTurn(Turn::ActorA)
{

}

void BattleController::PlayTurn()
{
    switch(m_currentTurn) {
        case Turn::ActorA : {
            if (m_eventState.isAction) { // TODO
                m_actorB.TakeDamage(m_actorA.GetStrength());
                m_currentTurn = Turn::ActorB; // SwitchTurn function ?
            }   
            break;
        }
        case Turn::ActorB : {
            m_actorA.TakeDamage(m_actorB.GetStrength());
            m_currentTurn = Turn::ActorA; // SwitchTurn function ?
            break;
        }
    }
    // Notify(ExitEvent::ExitWin);
}
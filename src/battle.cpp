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

BattleController::BattleController(const BattleActor actorA, const BattleActor actorB):
    m_actorA(actorA), m_actorB(actorB)
{

}

void BattleController::SetEventState(const BattleEventState eventState)
{
    m_eventState = eventState;
}

void BattleController::PlayTurn()
{

}
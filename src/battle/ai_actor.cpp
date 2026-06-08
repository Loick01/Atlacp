#include "battle/ai_actor.hpp"

AiActor::AiActor(const Team team, const UiKey& nameId, const UiKey& healthId, const UiKey& pathId, 
    const std::string name, const unsigned int health, const unsigned int turnSpeed) : 
BattleActor(team, nameId, healthId, pathId, name, health, turnSpeed)
{}

const BattleBehaviour& AiActor::GetBehaviour() const
{
    return m_behaviour;
}
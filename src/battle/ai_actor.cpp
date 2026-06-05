#include "battle/ai_actor.hpp"

AiActor::AiActor(const Team team, const ElementKey& nameId, const ElementKey& healthId, const ElementKey& pathId, 
    const std::string name, const unsigned int health, const unsigned int turnSpeed) : 
BattleActor(Team::Opponent, nameId, healthId, pathId, name, health, turnSpeed)
{}

const BattleBehaviour& AiActor::GetBehaviour() const
{
    return m_behaviour;
}
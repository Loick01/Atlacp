#include "behaviour.hpp"

EntityBehaviour::EntityBehaviour()
{

}

EntityBehaviour::~EntityBehaviour()
{
    
}

void RandomMovementBehaviour::FreeCase(Entity& entity)
{
    entity.OrderStartMovement(m_random.GetRandomDirection(), true);
}

void RandomMovementBehaviour::MovingCase(Entity& entity, const float delta_time)
{
    entity.OrderUpdateMovement(delta_time);
}

void RandomMovementBehaviour::OnStopCase(Entity& entity)
{
    entity.OrderStartMovement(m_random.GetRandomDirection(), false);
}
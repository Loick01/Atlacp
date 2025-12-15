#include "behaviour.hpp"

EntityBehaviour::EntityBehaviour()
{

}

EntityBehaviour::~EntityBehaviour()
{
    
}

void RandomBehaviour::FreeCase(Entity& entity)
{
    entity.OrderStartMovement(m_random.GetRandomDirection(), true);
}

void RandomBehaviour::MovingCase(Entity& entity, const float delta_time)
{
    entity.OrderUpdateMovement(delta_time);
}

void RandomBehaviour::OnStopCase(Entity& entity)
{
    entity.OrderStartMovement(m_random.GetRandomDirection(), false);
}

FollowEntityBehaviour::FollowEntityBehaviour(const Entity* tracked_entity, const float follower_speed):
    m_tracked_entity(tracked_entity)
{
    // Will need to check if entity != nullptr
    if (tracked_entity->GetSpeed() > follower_speed) // Tracked entity should not be faster than the entity who own this behaviour
        std::cout << "This behaviour should not be used here\n";
}

void FollowEntityBehaviour::FreeCase(Entity& entity)
{
    const EntityMovement movement = m_tracked_entity->GetCurrentMovement();
    const MapPosition delta_position = movement.GetStartPosition() - entity.GetMapPosition();
    const MapDirection direction = movement.GetDirectionFromMove(delta_position); // Could use a static function instead ?
    entity.OrderStartMovement(direction, true);
}

void FollowEntityBehaviour::MovingCase(Entity& entity, const float delta_time)
{
    entity.OrderUpdateMovement(delta_time);
}

void FollowEntityBehaviour::OnStopCase(Entity& entity)
{
    const EntityMovement movement = m_tracked_entity->GetCurrentMovement();
    const MapPosition delta_position = movement.GetStartPosition() - entity.GetMapPosition();
    const MapDirection direction = movement.GetDirectionFromMove(delta_position); // Could use a static function instead ?
    entity.OrderStartMovement(direction, false);
}
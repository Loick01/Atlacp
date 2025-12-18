#include "behaviour.hpp"

EntityBehaviour::EntityBehaviour()
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
    if (m_tracked_entity->GetState() != EntityState::Free){
        const EntityMovement movement = m_tracked_entity->GetCurrentMovement();
        const MapPosition delta_position = movement.GetStartPosition() - entity.GetMapPosition();
        const MapDirection direction = movement.GetDirectionFromMove(delta_position); // Could use a static function instead ?
        entity.OrderStartMovement(direction, true);
    }
}

void FollowEntityBehaviour::MovingCase(Entity& entity, const float delta_time)
{
    entity.OrderUpdateMovement(delta_time);
}

void FollowEntityBehaviour::OnStopCase(Entity& entity)
{
    if (m_tracked_entity->GetState() != EntityState::Free){
        const EntityMovement movement = m_tracked_entity->GetCurrentMovement();
        const MapPosition start_mp = movement.GetStartPosition();
        const MapPosition delta_position = movement.GetStartPosition() - entity.GetMapPosition();
        const MapDirection direction = movement.GetDirectionFromMove(delta_position); // Could use a static function instead ?
        entity.OrderStartMovement(direction, false);
    }else{
        entity.Reset();
    }
}

GoToBehaviour::GoToBehaviour(const MapPosition start_position, const MapPosition end_position, const Tilemap& tilemap):
    m_path_index(0)
{
    m_path = Pathfind::GetInstance().ComputePath(start_position, end_position, tilemap);
}

void GoToBehaviour::FreeCase(Entity& entity)
{
    if (m_path_index < m_path.size()){
        const MapPosition next_position = m_path[m_path_index];
        const MapPosition delta_position = next_position - entity.GetMapPosition();
        const MapDirection direction = entity.GetCurrentMovement().GetDirectionFromMove(delta_position); // Could use a static function instead ?
        entity.OrderStartMovement(direction, true);
    }
}

void GoToBehaviour::MovingCase(Entity& entity, const float delta_time)
{
    entity.OrderUpdateMovement(delta_time);
}

void GoToBehaviour::OnStopCase(Entity& entity)
{
    m_path_index++;
    if (m_path_index < m_path.size()){
        const MapPosition next_position = m_path[m_path_index];
        const MapPosition delta_position = next_position - entity.GetMapPosition();
        const MapDirection direction = entity.GetCurrentMovement().GetDirectionFromMove(delta_position); // Could use a static function instead ?
        entity.OrderStartMovement(direction, false);
    }else{
        entity.Reset();
    }
}
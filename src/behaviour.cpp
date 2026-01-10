#include "behaviour.hpp"

EntityBehaviour::EntityBehaviour()
{

}

RandomBehaviour::RandomBehaviour()
{
    m_delay = m_random.GetRandomFloat(0.5f, 5.f);
}

void RandomBehaviour::FreeCase(Entity& entity, const float delta_time)
{
    if (m_delay > 0.f) m_delay -= delta_time;
    else{
        entity.OrderStartMovement(m_random.GetRandomDirection(), true);
        // When I made the sprite orientation update even if the movement isn't valid, if the NPC is blocked in the 4 directions,
        // it will be updated every frame. I would need something else to handle that case
        if (entity.GetState() != EntityState::Free) // Generate a new delay only if the movement is valid (no collision)
            m_delay = m_random.GetRandomFloat(0.5f, 5.f);
    }
}

void RandomBehaviour::MovingCase(Entity& entity, const float delta_time)
{
    entity.OrderUpdateMovement(delta_time);
}

void RandomBehaviour::OnStopCase(Entity& entity)
{
    entity.Reset(); // Set state to Free where a new delay will be generated
}

FollowEntityBehaviour::FollowEntityBehaviour(const Entity* tracked_entity, const float follower_speed):
    m_tracked_entity(tracked_entity)
{
    // Will need to check if entity != nullptr
    if (tracked_entity->GetSpeed() > follower_speed) // Tracked entity should not be faster than the entity who own this behaviour
        std::cout << "This behaviour should not be used here\n";
}

void FollowEntityBehaviour::FreeCase(Entity& entity, const float delta_time)
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

void GoToBehaviour::FreeCase(Entity& entity, const float delta_time)
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
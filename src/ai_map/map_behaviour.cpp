#include "ai_map/map_behaviour.hpp"

#include "ai_map/pathfind.hpp"
#include "map/map_entity.hpp"
#include "tile/tilemap.hpp"

MapRandomBehaviour::MapRandomBehaviour()
{
    m_delay = m_random.GetRandomFloat(0.5f, 5.f);
}

void MapRandomBehaviour::FreeCase(MapEntity& entity, const float deltaTime)
{
    if (m_delay > 0.f) m_delay -= deltaTime;
    else{
        entity.OrderStartMovement(m_random.GetRandomDirection(), true);
        // When I made the sprite orientation update even if the movement isn't valid, if the NPC is blocked in the 4 directions,
        // it will be updated every frame. I would need something else to handle that case
        if (entity.GetState() != EntityState::Free) // Generate a new delay only if the movement is valid (no collision)
            m_delay = m_random.GetRandomFloat(0.5f, 5.f);
    }
}

void MapRandomBehaviour::MovingCase(MapEntity& entity, const float deltaTime)
{
    entity.OrderUpdateMovement(deltaTime);
}

void MapRandomBehaviour::OnStopCase(MapEntity& entity)
{
    entity.Reset(entity.GetCurrentMovement().GetDirection()); // Set state to Free where a new delay will be generated
}

MapFollowBehaviour::MapFollowBehaviour(const MapEntity* trackedEntity, const float followerWalkSpeed):
    m_trackedEntity(trackedEntity)
{
    if (trackedEntity == nullptr)
        throw std::invalid_argument("Tracked entity is nullptr\n");
    // Should also test m_runSpeed
    if (trackedEntity->GetWalkSpeed() > followerWalkSpeed) // Tracked entity should not be faster than the entity who own this behaviour
        throw std::invalid_argument("MapFollowBehaviour should not be used if tracked entity is faster than follower\n");
}

void MapFollowBehaviour::FreeCase(MapEntity& entity, const float deltaTime)
{
    if (m_trackedEntity->GetState() != EntityState::Free){
        const MapMovement movement = m_trackedEntity->GetCurrentMovement();
        const MapPosition deltaPosition = movement.GetStartPosition() - entity.GetMapPosition();
        const Direction direction = movement.GetDirectionFromMove(deltaPosition); // Could use a static function instead ?
        entity.SetIsRunning(m_trackedEntity->GetIsRunning());
        entity.OrderStartMovement(direction, true);
    }
}

void MapFollowBehaviour::MovingCase(MapEntity& entity, const float deltaTime)
{
    entity.OrderUpdateMovement(deltaTime);
}

void MapFollowBehaviour::OnStopCase(MapEntity& entity)
{
    if (m_trackedEntity->GetState() != EntityState::Free){
        const MapMovement movement = m_trackedEntity->GetCurrentMovement();
        const MapPosition deltaPosition = movement.GetStartPosition() - entity.GetMapPosition();
        const Direction direction = movement.GetDirectionFromMove(deltaPosition); // Could use a static function instead ?
        entity.SetIsRunning(m_trackedEntity->GetIsRunning());
        entity.OrderStartMovement(direction, false);
    }else{
        entity.Reset(entity.GetCurrentMovement().GetDirection());
    }
}

MapGoToBehaviour::MapGoToBehaviour(const MapPosition startPosition, const MapPosition endPosition, const Tilemap& tilemap):
    m_pathIndex(0)
{
    m_path = Pathfind::ComputePath(startPosition, endPosition, tilemap);
}

void MapGoToBehaviour::FreeCase(MapEntity& entity, const float deltaTime)
{
    if (m_pathIndex < m_path.size()){
        const MapPosition nextPosition = m_path[m_pathIndex];
        const MapPosition deltaPosition = nextPosition - entity.GetMapPosition();
        const Direction direction = entity.GetCurrentMovement().GetDirectionFromMove(deltaPosition); // Could use a static function instead ?
        // Use entity.SetIsRunning if the NPC need to run
        entity.OrderStartMovement(direction, true);
    }
}

void MapGoToBehaviour::MovingCase(MapEntity& entity, const float deltaTime)
{
    entity.OrderUpdateMovement(deltaTime);
}

void MapGoToBehaviour::OnStopCase(MapEntity& entity)
{
    m_pathIndex++;
    if (m_pathIndex < m_path.size()){
        const MapPosition nextPosition = m_path[m_pathIndex];
        const MapPosition deltaPosition = nextPosition - entity.GetMapPosition();
        const Direction direction = entity.GetCurrentMovement().GetDirectionFromMove(deltaPosition); // Could use a static function instead ?
        // Use entity.SetIsRunning if the NPC need to run
        entity.OrderStartMovement(direction, false);
    }else{
        entity.Reset(entity.GetCurrentMovement().GetDirection());
    }
}
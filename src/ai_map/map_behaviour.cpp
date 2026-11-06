#include "ai_map/map_behaviour.hpp"

#include "ai_map/pathfind.hpp"
#include "map/map_entity.hpp"
#include "tile/tilemap.hpp"

MapEntityBehaviour::MapEntityBehaviour():
    m_init(false)
{}

bool MapEntityBehaviour::IsInit() const
{
    return m_init;
}

MapRandomBehaviour::MapRandomBehaviour():
    MapEntityBehaviour()
{
    m_delay = m_random.GetRandomFloat(0.5f, 5.f);
    m_init = true;
}

void MapRandomBehaviour::FreeCase(MapEntity& entity, const float deltaTime)
{
    if (m_delay > 0.f) m_delay -= deltaTime;
    else{
        entity.OrderStartMovement(m_random.GetRandomDirection(), true);
        // TODO : When I made the sprite orientation update even if the movement isn't valid, if the NPC is blocked in the 4 directions,
        // it will be updated every frame. I would need something else to handle that case

        // Generate a new delay only if the movement is valid (no collision)
        // Don't need to test GetInteractionState() here, already to EntityInteractionState::None when calling Behaviour::FreeCase
        if (entity.GetMovementState() != EntityMovementState::Free)
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

MapFollowBehaviour::MapFollowBehaviour(const MapEntity* followerEntity, const MapEntity* trackedEntity):
    MapEntityBehaviour(), m_trackedEntity(trackedEntity)
{
    if (trackedEntity == nullptr) {
        Logger::GetInstance().AddLog(Level::L4, "Tracked entity is nullptr"); // TODO : Level
        return;
    }
    
    // Tracked entity should not be faster than the entity who own this behaviour
    if (trackedEntity->GetWalkSpeed() > followerEntity->GetWalkSpeed() || trackedEntity->GetRunSpeed() > followerEntity->GetRunSpeed()) {
        Logger::GetInstance().AddLog(Level::L4, "MapFollowBehaviour should not be used if tracked entity is faster than follower"); // TODO : Level
        return;
    }

    // This behaviour do not use pathfinding, it will be a different behaviour
    if (followerEntity->GetMapPosition().GetManhattanDistance(trackedEntity->GetMapPosition()) != 1) {
        Logger::GetInstance().AddLog(Level::L4, "MapFollowBehaviour can start only when the tracked entity is next to the follower"); // TODO : Level
        return;
    }

    m_init = true;
}

void MapFollowBehaviour::FreeCase(MapEntity& entity, const float deltaTime)
{
    if (m_trackedEntity->GetMovementState() != EntityMovementState::Free && m_trackedEntity->GetInteractionState() == EntityInteractionState::None){
        const MapMovement movement = m_trackedEntity->GetCurrentMovement();
        const MapPosition deltaPosition = movement.GetStartPosition() - entity.GetMapPosition();
        const Direction direction = movement.GetDirectionFromMove(deltaPosition); // TODO : Could use a static function instead ?
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
    if (m_trackedEntity->GetMovementState() != EntityMovementState::Free && m_trackedEntity->GetInteractionState() == EntityInteractionState::None){
        const MapMovement movement = m_trackedEntity->GetCurrentMovement();
        const MapPosition deltaPosition = movement.GetStartPosition() - entity.GetMapPosition();
        const Direction direction = movement.GetDirectionFromMove(deltaPosition); // TODO : Could use a static function instead ?
        entity.SetIsRunning(m_trackedEntity->GetIsRunning());
        entity.OrderStartMovement(direction, false);
    }else{
        entity.Reset(entity.GetCurrentMovement().GetDirection());
    }
}

MapGoToBehaviour::MapGoToBehaviour(const MapPosition startPosition, const MapPosition endPosition, const Tilemap& tilemap):
    MapEntityBehaviour(), m_pathIndex(0)
{
    m_path = Pathfind::ComputePath(startPosition, endPosition, tilemap);
    // if (m_path.empty())
    //     return;
    m_init = true; // TODO++ : m_init should be true only if a path exists
}

bool MapGoToBehaviour::IsDone() const
{
    return m_pathIndex >= m_path.size();
}

void MapGoToBehaviour::FreeCase(MapEntity& entity, const float deltaTime)
{
    if (!IsDone()){
        const MapPosition nextPosition = m_path[m_pathIndex];
        const MapPosition deltaPosition = nextPosition - entity.GetMapPosition();
        const Direction direction = entity.GetCurrentMovement().GetDirectionFromMove(deltaPosition); // TODO : Could use a static function instead ?
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
    if (!IsDone()){
        const MapPosition nextPosition = m_path[m_pathIndex];
        const MapPosition deltaPosition = nextPosition - entity.GetMapPosition();
        const Direction direction = entity.GetCurrentMovement().GetDirectionFromMove(deltaPosition); // TODO : Could use a static function instead ?
        // Use entity.SetIsRunning if the NPC need to run
        entity.OrderStartMovement(direction, false);
    }else{
        Notify(UselessEvent::None);
        entity.Reset(entity.GetCurrentMovement().GetDirection());
    }
}

MapIdleBehaviour::MapIdleBehaviour(MapEntity& entity, const Direction direction):
    MapEntityBehaviour(), m_direction(direction)
{
    entity.SetOrientation(m_direction);
    m_init = true;
}

void MapIdleBehaviour::FreeCase(MapEntity& entity, const float deltaTime)
{}

void MapIdleBehaviour::MovingCase(MapEntity& entity, const float deltaTime)
{}

void MapIdleBehaviour::OnStopCase(MapEntity& entity)
{}
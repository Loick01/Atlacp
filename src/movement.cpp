#include "movement.hpp"

EntityMovement::EntityMovement():
    m_direction(MapDirection::None), m_move(MapPosition{0, 0})
{

}

MapPosition EntityMovement::GetMove() const
{
    return m_move;
}

MapDirection EntityMovement::GetDirection() const
{
    return m_direction;
}

MapDirection EntityMovement::GetOppositeDirection() const
{
    return static_cast<MapDirection>(((int)m_direction+2)%4);
}

MapPosition EntityMovement::GetStartPosition() const
{
    return m_startMapPosition;
}

ScenePosition EntityMovement::GetScenePosition() const
{
    return m_startPosition + (m_endPosition - m_startPosition) * m_progress; // Should be in Interpolation struct ?
}

EntityState EntityMovement::UpdateProgress(const float speed, const float deltaTime)
{
    m_progress += speed * deltaTime;
    m_progress = std::min(1.0f, m_progress); 
    EntityState newState = m_progress == 1.f ? EntityState::OnStop : EntityState::Moving;
    return newState;
}

MapPosition EntityMovement::GetMoveFromDirection(const MapDirection direction) const
{
    switch(direction){
        case MapDirection::Up:
            return MapPosition{0, -1};
        case MapDirection::Down:
            return MapPosition{0, 1};
        case MapDirection::Left:
            return MapPosition{-1, 0};
        case MapDirection::Right:
            return MapPosition{1, 0};
        default: // Will throw error ? (for MapDirection::None)
            return MapPosition{0, 0}; 
    }
}

MapDirection EntityMovement::GetDirectionFromMove(const MapPosition move) const
{
    if (move.x==0){
        if (move.y==-1) return MapDirection::Up;
        else if (move.y==1) return MapDirection::Down;
    }else{
        if (move.x==-1) return MapDirection::Left;
        else if (move.x==1) return MapDirection::Right;
    }
    return MapDirection::None; // Will throw error ?
}

void EntityMovement::DefineMovement(const MapDirection direction)
{
    m_direction = direction;
    m_move = GetMoveFromDirection(direction);
}

void EntityMovement::Initialize(const int tileSize, const MapPosition startPosition, const MapPosition endPosition)
{
    m_startMapPosition = startPosition;
    m_startPosition = startPosition.ToScenePosition(tileSize);
    m_endPosition = endPosition.ToScenePosition(tileSize);
    m_progress = 0.;
}
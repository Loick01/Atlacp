#include "map/movement.hpp"

EntityMovement::EntityMovement():
    m_direction(Direction::None), m_move(MapPosition{0, 0})
{

}

MapPosition EntityMovement::GetMove() const
{
    return m_move;
}

Direction EntityMovement::GetDirection() const
{
    return m_direction;
}

Direction EntityMovement::GetOppositeDirection() const
{
    return static_cast<Direction>(((int)m_direction+2)%4);
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

MapPosition EntityMovement::GetMoveFromDirection(const Direction direction) const
{
    switch(direction){
        case Direction::Up:
            return MapPosition{0, -1};
        case Direction::Down:
            return MapPosition{0, 1};
        case Direction::Left:
            return MapPosition{-1, 0};
        case Direction::Right:
            return MapPosition{1, 0};
        default: // Will throw error ? (for Direction::None)
            return MapPosition{0, 0}; 
    }
}

Direction EntityMovement::GetDirectionFromMove(const MapPosition move) const
{
    if (move.x==0){
        if (move.y==-1) return Direction::Up;
        else if (move.y==1) return Direction::Down;
    }else{
        if (move.x==-1) return Direction::Left;
        else if (move.x==1) return Direction::Right;
    }
    return Direction::None; // Will throw error ?
}

void EntityMovement::DefineMovement(const Direction direction)
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
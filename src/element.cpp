#include "element.hpp"

MapMovement::MapMovement():
    m_direction(MapDirection::None), m_move(MapPosition{0, 0})
{

}

MapMovement::~MapMovement()
{

}

MapPosition MapMovement::GetMove() const
{
    return m_move;
}

MapDirection MapMovement::GetDirection() const
{
    return m_direction;
}

ScenePosition MapMovement::GetScenePosition() const
{
    return m_start_position + (m_end_position - m_start_position) * m_progress; // Should be in Interpolation struct ?
}

ElementState MapMovement::UpdateProgress(const float speed, const float delta_time)
{
    m_progress += speed * delta_time;
    m_progress = std::min(1.0f, m_progress); 
    ElementState new_state = m_progress == 1.f ? ElementState::StopMoving : ElementState::Moving;
    return new_state;
}

void MapMovement::ResetProgress()
{
    m_progress = 0.f;
}

void MapMovement::DefineMovement(MapDirection direction)
{
    m_direction = direction;
    switch(direction){
        case MapDirection::Up:
            m_move = MapPosition{0, -1};
            break;
        case MapDirection::Down:
            m_move = MapPosition{0, 1};
            break;
        case MapDirection::Left:
            m_move = MapPosition{-1, 0};
            break;
        case MapDirection::Right:
            m_move = MapPosition{1, 0};
            break;
    }
}

void MapMovement::Initialize(const int tile_size, const MapPosition start_position, const MapPosition end_position)
{
    m_start_position = start_position.ToScenePosition(tile_size);
    m_end_position = end_position.ToScenePosition(tile_size);
    m_progress = 0.;
}

MapElement::MapElement(const FileReader* file_reader, const std::string& sprite_filepath, Tilemap* tilemap, const float speed):
    m_tilemap(tilemap), m_state(ElementState::Free), m_speed(speed), m_animation(file_reader, sprite_filepath)
{
    
}

MapElement::~MapElement()
{

}

void MapElement::SetMapPosition(const MapPosition mp)
{
    m_map_position = mp;
}

void MapElement::StartMovement(const MapMovement movement, const bool is_first_movement, const bool can_exit_map)
{
    MapPosition new_pos = m_map_position + movement.GetMove();
    if (m_tilemap->IsFreePosition(new_pos, can_exit_map)){
        m_current_movement = movement;
        m_animation.Initialize(movement.GetDirection(), is_first_movement);
        m_state = ElementState::Moving;
        int tile_size = m_tilemap->GetTileSize();
        m_current_movement.Initialize(tile_size, m_map_position, new_pos);
        m_tilemap->FreePosition(m_map_position);
        m_tilemap->TakePosition(new_pos);
        m_map_position = new_pos;
    } // Should reset here the animation to use idle sprite when there is a collision ?
}

ScenePosition MapElement::ContinueMovement(const float delta_time)
{
    m_state = m_current_movement.UpdateProgress(m_speed, delta_time);
    m_animation.ContinueAnimation(delta_time);
    return m_current_movement.GetScenePosition();
}
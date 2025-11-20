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

MapElement::MapElement(Tilemap* tilemap, const MapPosition p, const float speed):
    m_tilemap(tilemap), m_map_position(p), m_is_free(true), m_speed(speed)
{

}

MapElement::~MapElement()
{

}

void MapElement::StartMovement(const MapPosition movement)
{
    MapPosition new_pos = m_map_position + movement;
    if (m_tilemap->CheckNewPosition(new_pos)){
        m_is_free = false;

        int tile_size = m_tilemap->GetTileSize();
        
        // Should be in MapMovement class 
        m_start_position = m_map_position.ToScenePosition(tile_size);
        m_end_position = new_pos.ToScenePosition(tile_size);
        m_progress = 0.;
        m_last_time = SDL_GetTicks();

        m_map_position = new_pos;
    }
}
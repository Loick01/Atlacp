#include "player.hpp"

Player::Player(Tilemap* tilemap, TextureController* texture_controller, const MapEventController* event_controller,
    const std::string& sprite_filepath, Camera* camera):
    Drawable(texture_controller, sprite_filepath, camera, ScenePosition{0,0}), MapElement({0,0}), m_event_controller(event_controller),
    m_tilemap(tilemap), m_tile_size(tilemap->GetTileSize())
{
    m_position = ScenePosition{m_map_position.x*m_tile_size, m_map_position.y*m_tile_size};
    m_camera->LookAt(m_position);
}

Player::~Player()
{
    m_texture_controller->DeleteTexture(m_texture_key);
}

void Player::GetNewPosition(const MapPosition movement) // Should be in MapElement class ?
{
    MapPosition new_pos = m_map_position + movement;
    if (m_tilemap->CheckNewPosition(new_pos)){
        m_map_position = new_pos;
        m_position = ScenePosition{m_map_position.x*m_tile_size, m_map_position.y*m_tile_size};
        m_camera->LookAt(m_position);
    }
}

void Player::Update()
{
    const MapMovement movement = m_event_controller->HandlePlayerEvent();

    switch(movement){
        case MapMovement::None:
            break;
        case MapMovement::Up:
            GetNewPosition(MapPosition{0,-1});
            break;
        case MapMovement::Down:
            GetNewPosition(MapPosition{0,1});
            break;
        case MapMovement::Left:
            GetNewPosition(MapPosition{-1,0});
            break;
        case MapMovement::Right:
            GetNewPosition(MapPosition{1,0});
            break;
    }
}
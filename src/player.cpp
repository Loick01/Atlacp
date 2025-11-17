#include "player.hpp"

Player::Player(Tilemap* tilemap, TextureController* texture_controller, const MapEventController* event_controller,
    const std::string& sprite_filepath, Camera* camera):
    Drawable(texture_controller, sprite_filepath, camera, ScenePosition{0,0}), MapElement({10,10}), m_event_controller(event_controller),
    m_tilemap(tilemap), m_tile_size(tilemap->GetTileSize())
{
    // Set the camera position to have the player in the middle of screen (without checking the tilemap bound)
    //const ScreenPosition sp = m_map_position.ToScreenPosition(m_tile_size) /*+ m_tile_size/2*/;
    //m_camera->SetCameraPosition(window_center-sp); // Tilemap and player share the same drawing offset thanks to camera

    m_position = ScenePosition{m_map_position.x*m_tile_size, m_map_position.y*m_tile_size};
}

Player::~Player()
{
    m_texture_controller->DeleteTexture(m_texture_key);
}

void Player::GetNewPosition(const MapPosition movement) // Should be in MapElement class ?
{
    MapPosition new_pos = m_map_position + movement;
    if (m_tilemap->CheckNewPosition(new_pos)){

        /*
        int axis_position, axis_movement; // Previous position on the axis concerned by the movement
        if (movement.x != 0){
            axis_position = m_map_position.x;
            axis_movement = movement.x;
        }else{
            axis_position = m_map_position.y;
            axis_movement = movement.y;
        }
        if (m_tilemap->CanMoveCamera(axis_position, axis_position+axis_movement)){
            const ScreenPosition sp = (movement*-1).ToScreenPosition(m_tile_size); // m_tile_size must be equal to the same tile_size in Tilemap, if not use tilemap->GetTileSize()
            m_camera->MoveCameraPosition(sp);
        }
            */
        m_map_position = new_pos; // Update the position only after using the previous one (for axis_position)
        m_position = ScenePosition{m_map_position.x*m_tile_size, m_map_position.y*m_tile_size};
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
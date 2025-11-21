#include "player.hpp"

Player::Player(Tilemap* tilemap, TextureController* texture_controller, const MapEventController* event_controller,
    const std::string& sprite_filepath, Camera* camera, const float speed):
    Drawable(texture_controller, sprite_filepath, camera, ScenePosition{0,0}), MapElement(tilemap, {10,10}, speed), m_event_controller(event_controller)
{
    m_position = m_map_position.ToScenePosition(tilemap->GetTileSize());
    LookMe();
}

Player::~Player()
{
    m_texture_controller->DeleteTexture(m_texture_key);
}

void Player::Update()
{
    if (m_is_free){
        const MapMovement movement = m_event_controller->HandlePlayerEvent();

        switch(movement.GetDirection()){
            case MapDirection::None:
                break;
            default:
                m_current_movement = movement;
                StartMovement(m_current_movement);
                break;
        }
    }else{
        m_position = ContinueMovement(m_current_movement);
        LookMe();
    }
}
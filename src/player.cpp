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
                StartMovement(movement.GetMove());
                break;
        }
    }else{
        uint32_t currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - m_last_time) / 1000.f;
        m_last_time = currentTime;

        m_progress += m_speed * deltaTime;
        if (m_progress >= 1.0){
            m_progress = 1.0;
            m_is_free = true;
        }
        m_position = m_start_position + (m_end_position - m_start_position) * m_progress;
        LookMe();
    }
}
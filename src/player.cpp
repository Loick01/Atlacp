#include "player.hpp"

Player::Player(Tilemap* tilemap, TextureController* texture_controller, const MapEventController* event_controller,
    const std::string& sprite_filepath, Camera* camera, const float speed):
    Drawable(texture_controller, sprite_filepath, camera, ScenePosition{0,0}), MapElement({10,10}, speed), m_event_controller(event_controller),
    m_tilemap(tilemap), m_tile_size(tilemap->GetTileSize())
{
    m_position = m_map_position.ToScenePosition(m_tile_size);
    LookMe();
}

Player::~Player()
{
    m_texture_controller->DeleteTexture(m_texture_key);
}

void Player::StartMovement(const MapPosition movement) // Should be in MapElement class (npc will also move) ?
{
    MapPosition new_pos = m_map_position + movement;
    if (m_tilemap->CheckNewPosition(new_pos)){
        m_is_free = false;
        
        // Will be removed (4 lines)
        m_start_pos = m_map_position.ToScenePosition(m_tile_size);
        m_end_pos = new_pos.ToScenePosition(m_tile_size);
        m_progress = 0.;
        m_last_time = SDL_GetTicks();

        m_map_position = new_pos;
    }
}

void Player::Update()
{
    if (m_is_free){
        const MapMovement movement = m_event_controller->HandlePlayerEvent();

        switch(movement){
            case MapMovement::None:
                break;
            case MapMovement::Up:
                StartMovement(MapPosition{0,-1});
                break;
            case MapMovement::Down:
                StartMovement(MapPosition{0,1});
                break;
            case MapMovement::Left:
                StartMovement(MapPosition{-1,0});
                break;
            case MapMovement::Right:
                StartMovement(MapPosition{1,0});
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
        m_position = m_start_pos + (m_end_pos - m_start_pos) * m_progress;
        LookMe();
    }
}
#include "player.hpp"

Player::Player(Tilemap* tilemap, TextureController* texture_controller, const MapEventController* event_controller,
    const std::string& sprite_filepath, Camera* camera, const float speed):
    Drawable(texture_controller, sprite_filepath, camera, ScenePosition{0,0}), MapElement(tilemap, speed), m_event_controller(event_controller)
{
    const MapPosition spawn = tilemap->GetSpawnPosition();
    if (spawn.x != -1 && spawn.y != -1)
        m_map_position = spawn;
    else { // This should not happen
        m_map_position = MapPosition{0, 0};
        std::cout << "A spawn position must be defined for the first loaded map in tilemap (check world file)\n";
    }

    m_position = m_map_position.ToScenePosition(tilemap->GetTileSize());
    LookMe();

    // Class Animated
    m_anim_step = 4;
    m_frame_duration = 0.3f;
    m_is_first_movement = true;
    m_anim_index = 0;
    m_sprite_size = tilemap->GetTileSize();
    // Will use the first element in std::vector<Pair<int>> to initialize the first sprite
    m_src = Pair<int>{0, 0};
    m_spritesheet_size = Pair<int>{2, 2};
}

Player::~Player()
{
    m_texture_controller->DeleteTexture(m_texture_key);
}

void Player::DrawTexture() const
{
    const SDL_Rect src{m_src.x, m_src.y, m_sprite_size, m_sprite_size};
    const ScenePosition camera_position = m_camera->GetCameraPosition();
    const SDL_Rect dst{m_position.x-camera_position.x, m_position.y-camera_position.y, m_sprite_size, m_sprite_size};
    m_texture_controller->RenderTexture(m_texture_key, src, dst);
}

void Player::Update()
{
    switch (m_state){
        case ElementState::Free:
        {
            const MapMovement movement = m_event_controller->HandlePlayerEvent();

            switch(movement.GetDirection()){
                case MapDirection::None:
                    break;
                default:
                    if (m_is_first_movement){ // When this is the first movement since the player release the control
                        m_is_first_movement = false;
                        m_count = 0.f;
                        m_anim_index = 1; // Important --> Force to don't use the idle sprite 
                        m_last_time = SDL_GetTicks();
                    }
                    m_current_movement = movement;
                    StartMovement(m_current_movement);
                    break;
            }
            break;
        }

        case ElementState::Moving:
        {
            m_position = ContinueMovement(m_current_movement);

            uint32_t current_time = SDL_GetTicks(); // Should use a Time class (same code in MapMovement::UpdateProgress)
            float deltaTime = (current_time - m_last_time) / 1000.f;
            m_last_time = current_time;
            m_count += deltaTime;
            if (m_count >= m_frame_duration){
                m_count -= m_frame_duration;
                m_anim_index = (m_anim_index+1)%m_anim_step;
            }
            m_src = Pair<int>{(m_anim_index%m_spritesheet_size.x)*m_sprite_size, (m_anim_index/m_spritesheet_size.x)*m_sprite_size};

            LookMe();
            break;
        }

        case ElementState::StopMoving: // Enter this case at the end of the current movement, then go to case Moving if still control the player, else go to case Free (and resetting the animation)
        {
            const MapMovement movement = m_event_controller->HandlePlayerEvent();

            switch(movement.GetDirection()){
                case MapDirection::None: // No more movement, animation must reset
                {
                    m_is_first_movement = true; 
                    m_anim_index = 0;
                    m_src = Pair<int>{(m_anim_index%m_spritesheet_size.x)*m_sprite_size, (m_anim_index/m_spritesheet_size.x)*m_sprite_size};
                    m_state = ElementState::Free;
                    break;
                }
                default:
                    m_current_movement = movement;
                    StartMovement(m_current_movement);
                    break;
            }
            break;
        }

        default: // Should not happen
            break; 
    }
}
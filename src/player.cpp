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
}

Player::~Player()
{
    m_texture_controller->DeleteTexture(m_texture_key);
}

void Player::DrawTexture() const
{
    Pair<int> sprite = m_animation.GetCurrentSprite();
    const int sprite_size = m_animation.GetSpriteSize();

    const SDL_Rect src{sprite.x, sprite.y, sprite_size, sprite_size};
    const ScenePosition camera_position = m_camera->GetCameraPosition();
    const SDL_Rect dst{m_position.x-camera_position.x, m_position.y-camera_position.y, sprite_size, sprite_size};
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
                    m_animation.Initialize();
                    m_current_movement = movement;
                    StartMovement(m_current_movement);
                    break;
            }
            break;
        }

        case ElementState::Moving:
        {
            m_position = ContinueMovement(m_current_movement);
            m_animation.ContinueAnimation();
            LookMe();
            break;
        }

        case ElementState::StopMoving: // Enter this case at the end of the current movement
        {
            const MapMovement movement = m_event_controller->HandlePlayerEvent();

            switch(movement.GetDirection()){
                case MapDirection::None: // No more movement, animation must reset
                {
                    m_animation.Reset();
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
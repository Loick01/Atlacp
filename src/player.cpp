#include "player.hpp"

Player::Player(const FileReader* file_reader, Tilemap* tilemap, TextureController* texture_controller, const GameplayEventController* event_controller,
    const std::string& sprite_filepath, Camera* camera, const float speed):
    Drawable(texture_controller, sprite_filepath+".png", camera, ScenePosition{0,0}), MapElement(file_reader, sprite_filepath, tilemap, speed),
    m_event_controller(event_controller) // Remove +".png" if I create RessourceFile struct ?
{
    const MapPosition spawn = tilemap->GetSpawnPosition();
    if (spawn.x != -1 && spawn.y != -1)
        m_map_position = spawn;
    else { // This should not happen
        m_map_position = MapPosition{0, 0};
        std::cout << "A spawn position must be defined for the first loaded map in tilemap (check world file)\n";
    }

    const Pair<int> sprite_size = m_animation.GetSpriteSize();
    m_texture_width = sprite_size.x;
    m_texture_height = sprite_size.y;
    m_display_offset = ScenePosition{(m_texture_width-tilemap->GetTileSize())/2, m_texture_height-tilemap->GetTileSize()};
    m_position = GetFinalDrawingPosition(m_map_position.ToScenePosition(tilemap->GetTileSize()));
    LookMe();
}

Player::~Player()
{
    m_texture_controller->DeleteTexture(m_texture_key);
}

// Rename
ScenePosition Player::GetFinalDrawingPosition(const ScenePosition sp) const
{
    return (sp-m_display_offset)*m_camera->GetZoom();
}

void Player::DrawTexture() const
{
    const Pair<int> sprite = m_animation.GetCurrentSprite(); 
    const SDL_Rect src{sprite.x, sprite.y, m_texture_width, m_texture_height};
    const ScenePosition camera_position = m_camera->GetCameraPosition();
    const float zoom = m_camera->GetZoom();
    const SDL_Rect dst{m_position.x-camera_position.x, m_position.y-camera_position.y, m_texture_width*zoom, m_texture_height*zoom};
    m_texture_controller->RenderTexture(m_texture_key, src, dst);
}

void Player::Update(const float delta_time)
{
    switch (m_state){
        case ElementState::Free:
        {
            const MapMovement movement = m_event_controller->HandlePlayerEvent();

            switch(movement.GetDirection()){
                case MapDirection::None:
                    break;
                default:
                    StartMovement(movement, true);
                    LookMe(); // Important : Will clamp camera position, which is necessary to avoid negative index when culling (because of negative camera position)
                    break;
            }
            break;
        }

        case ElementState::Moving:
        {
            m_position = GetFinalDrawingPosition(ContinueMovement(delta_time));
            LookMe();
            break;
        }

        case ElementState::StopMoving: // Enter this case at the end of the current movement
        {
            const MapMovement movement = m_event_controller->HandlePlayerEvent();

            switch(movement.GetDirection()){
                case MapDirection::None:
                {
                    m_animation.Reset();
                    m_state = ElementState::Free;
                    break;
                }
                default:
                    StartMovement(movement, false);
                    LookMe(); // Same reason than case ElementState::Free
                    break;
            }
            break;
        }

        default: // Should not happen
            break; 
    }
}
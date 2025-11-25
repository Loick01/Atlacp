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
    m_step = 1./m_anim_step;
    m_sprite_size = tilemap->GetTileSize();
    // Will use the first element in std::vector<Pair<int>> to initialize the first sprite
    m_src = Pair<int>{0, 0};
    m_spritesheet_size = Pair<int>{2, 2};
}

Player::~Player()
{
    m_texture_controller->DeleteTexture(m_texture_key);
}

/*
void Player::DrawTexture() const
{
    const SDL_Rect src{m_src.x, m_src.y, m_sprite_size, m_sprite_size};
    const ScenePosition camera_position = m_camera->GetCameraPosition();
    const SDL_Rect dst{m_position.x-camera_position.x, m_position.y-camera_position.y, m_sprite_size, m_sprite_size};
    m_texture_controller->RenderTexture(m_texture_key, src, dst);
}
*/

void Player::DrawTexture() const
{
    const SDL_Rect src{m_src.x, m_src.y, m_sprite_size, m_sprite_size};
    const ScenePosition camera_position = m_camera->GetCameraPosition();
    const SDL_Rect dst{m_position.x-camera_position.x, m_position.y-camera_position.y, m_sprite_size, m_sprite_size};
    m_texture_controller->RenderTexture(m_texture_key, src, dst);
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

        if (m_is_free) m_current_movement.ResetProgress(); // When the movement just finishes, the progress must be set to 0, so the initial sprite will be drawn
        // Update the player sprite based on the progress of the movement(will be in Animated)
        int index = m_current_movement.GetProgress()/m_step;
        m_src = Pair<int>{(index%m_spritesheet_size.x)*m_sprite_size, (index/m_spritesheet_size.x)*m_sprite_size};

        LookMe();
    }
}
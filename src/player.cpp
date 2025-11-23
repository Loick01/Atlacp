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
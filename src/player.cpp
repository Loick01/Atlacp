#include "player.hpp"

Player::Player(const FileReader* file_reader, Tilemap* tilemap, TextureController* texture_controller, const GameplayEventController* event_controller,
    const std::string& sprite_filepath, Camera* camera, const float speed):
    Entity(texture_controller, sprite_filepath, camera, file_reader, tilemap, speed), m_event_controller(event_controller)
{
    const MapPosition spawn = tilemap->GetSpawnPosition();
    if (spawn.x != -1 && spawn.y != -1)
        SetMapPosition(spawn);
    else { // This should not happen
        SetMapPosition(MapPosition{0, 0});
        std::cout << "A spawn position must be defined for the first loaded map in tilemap (check world file)\n";
    }
    tilemap->TakePosition(m_map_position); // Should be in Entity ?
    m_position = GetFinalDrawingPosition(m_map_position.ToScenePosition(tilemap->GetTileSize()));
    LookMe();
}

Player::~Player()
{
    m_texture_controller->DeleteTexture(m_texture_key);
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
                    StartMovement(movement, true, true);
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
                    StartMovement(movement, false, true);
                    LookMe(); // Same reason than case ElementState::Free
                    break;
            }
            break;
        }

        default: // Should not happen
            break; 
    }
}
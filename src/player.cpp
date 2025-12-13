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
    const MapPosition mp = GetMapPosition();
    tilemap->TakePosition(mp); // Should be in Entity ?
    m_position = GetFinalDrawingPosition(mp.ToScenePosition(tilemap->GetTileSize()));
    LookMe();
}

Player::~Player()
{
    m_texture_controller->DeleteTexture(m_texture_key);
}

void Player::Update(const float delta_time)
{
    switch (GetState()){
        case EntityState::Free:
        {
            const EntityMovement movement = m_event_controller->HandlePlayerEvent();

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

        case EntityState::Moving:
        {
            m_position = GetFinalDrawingPosition(ContinueMovement(delta_time));
            LookMe();
            break;
        }

        case EntityState::StopMoving: // Enter this case at the end of the current movement
        {
            const EntityMovement movement = m_event_controller->HandlePlayerEvent();

            switch(movement.GetDirection()){
                case MapDirection::None:
                {
                    Reset();
                    break;
                }
                default:
                    StartMovement(movement, false, true);
                    LookMe(); // Same reason than case EntityState::Free
                    break;
            }
            break;
        }

        default: // Should not happen
            break; 
    }
}
#include "player.hpp"

Player::Player(const FileReader& fileReader, Tilemap& tilemap, TextureController& textureController,
    const std::string& spriteFilepath, Camera& camera, const float walkSpeed, const float runSpeed):
    Entity(textureController, spriteFilepath, camera, fileReader, tilemap, walkSpeed, runSpeed)
{
    const MapPosition spawn = tilemap.GetSpawnPosition();
    if (spawn.x != -1 && spawn.y != -1)
        SetMapPosition(spawn);
    else { // This should not happen
        SetMapPosition(MapPosition{0, 0});
        std::cout << "A spawn position must be defined for the first loaded map (defined in world file)\n";
    }
    const MapPosition mp = GetMapPosition();
    tilemap.TakePosition(mp); // Should be in Entity (currently not possible because spawn position if defined in Player constructor)
    m_position = GetFinalDrawingPosition(mp.ToScenePosition(tilemap.GetTileSize()));
    LookMe();
}

void Player::SetEventInfo(const GameplayEventInfo eventInfo)
{
    m_eventInfo = eventInfo;
}

void Player::Update(const float deltaTime)
{
    switch (GetState()){ // This code has the same structure than NPC::Update, I think I can merge it in Entity::Update
        case EntityState::Free:
        {
            if (m_eventInfo.isInteracting){
                // Warning : If the player has not moved once, direction is None by default (should initialize it in Entity constructor) 
                const MapDirection direction = GetCurrentMovement().GetDirection(); // Previous movement direction (can't use m_eventInfo.direction which is reset to None)
                OrderInteraction(direction);
                break;
            }
            const MapDirection direction = m_eventInfo.direction;
            switch(direction){
                case MapDirection::None:
                    break;
                default:
                    SetIsRunning(m_eventInfo.isRunning);
                    OrderStartMovement(direction, true, true);
                    LookMe(); // Important : Will clamp camera position, which is necessary to avoid negative index when culling (because of negative camera position)
                    break;
            }
            break;
        }

        case EntityState::Moving:
        {
            OrderUpdateMovement(deltaTime);
            LookMe();
            break;
        }

        case EntityState::OnStop: // Enter this case at the end of the current movement
        {
            Notify(EntityEvent::SortEntity); // Will sort the entities rendered by the Scene
            const MapDirection direction = m_eventInfo.direction;
            switch(direction){
                case MapDirection::None:
                {
                    Reset();
                    break;
                }
                default:
                    SetIsRunning(m_eventInfo.isRunning);
                    OrderStartMovement(direction, false, true);
                    LookMe(); // Same reason than case EntityState::Free
                    break;
            }
            break;
        }

        default: // Should not happen
            break; 
    }
}
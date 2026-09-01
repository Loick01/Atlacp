#include "map/player.hpp"

#include "tile/tilemap.hpp"

Player::Player(const FileReader& fileReader, Tilemap& tilemap, TextureController& textureController,
    const std::string& spriteFilepath, const float walkSpeed, const float runSpeed, const float cameraZoom):
    MapEntity(textureController, spriteFilepath, fileReader, tilemap, Direction::Down, walkSpeed, runSpeed, cameraZoom)
{
    const MapPosition spawn = tilemap.GetSpawnPosition();
    if (spawn.x != -1 && spawn.y != -1)
        SetMapPosition(spawn);
    else {
        // SetMapPosition(MapPosition{0, 0});
        throw std::runtime_error("A spawn position must be defined for the first loaded map (defined in world file)");
    }
    const MapPosition mp = GetMapPosition();
    tilemap.TakePosition(mp); // Should be in MapEntity (currently not possible because spawn position if defined in Player constructor)
    m_position = GetFinalDrawingPosition(mp.ToScenePosition(tilemap.GetTileSize()));
}

void Player::Update(const float deltaTime)
{
    const EntityInteractionState interactionState = GetInteractionState();
    if (interactionState != EntityInteractionState::None) {
        switch (interactionState) { // Maybe I will merge InteractionController and TriggerController, so this switch won't be needed anymore
            case EntityInteractionState::Interacting: {
                if (m_eventState.isInteracting){
                    ContinueInteraction();
                    break;
                }
                break;
            }

            case EntityInteractionState::Triggering: {
                if (m_eventState.isInteracting){
                    ContinueTrigger();
                    break;
                }
                break;
            }

            default:
                throw std::runtime_error("Unknown EntityInteractionState value");
                break; 
        }
        return;
    }

    switch (GetMovementState()){ // This code has the same structure than NPC::Update, I think I can merge it in MapEntity::Update
        case EntityMovementState::Free:
        {
            if (m_eventState.isInteracting){
                // Warning : If the player has not moved once, direction is None by default (should initialize it in MapEntity constructor) 
                const Direction direction = GetCurrentMovement().GetDirection(); // Previous movement direction (can't use m_eventState.mapDirection which is reset to None)
                OrderInteraction(direction);
                break;
            }
            const Direction direction = m_eventState.mapDirection;
            switch(direction){
                case Direction::None:
                    break;
                default:
                    SetIsRunning(m_eventState.isRunning);
                    OrderStartMovement(direction, true, true);
                    break;
            }
            break;
        }

        case EntityMovementState::Moving:
        {
            OrderUpdateMovement(deltaTime);
            break;
        }

        case EntityMovementState::OnStop: // Enter this case at the end of the current movement
        {
            Direction direction = m_eventState.mapDirection;
            
            Notify(EntityEvent::HasMoved); // Will set m_interactionState to Triggering if the MapEntity is on a trigger tile
            if (GetInteractionState() == EntityInteractionState::Triggering) {
                SetOrientation(GetCurrentMovement().GetDirection()); // Reset the animation when entering the trigger
                break;
            }

            switch(direction){
                case Direction::None:
                {
                    Reset(GetCurrentMovement().GetDirection()); // Previous movement direction (can't use m_eventState.mapDirection which is reset to None)
                    break;
                }
                default:
                    SetIsRunning(m_eventState.isRunning);
                    OrderStartMovement(direction, false, true);
                    break;
            }
            break;
        }

        default:
            throw std::runtime_error("Unknown EntityMovementState value");
            break; 
    }
}
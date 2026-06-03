#include "map/npc.hpp"

#include "tile/tilemap.hpp"

NPC::NPC(const FileReader& fileReader, Tilemap& tilemap, TextureController& textureController, const MapEntity* trackedEntity, // MapEntity parameter use for follow behaviour, should not be here
    const std::string& spriteFilepath, Camera& camera, const MapPosition position, const float walkSpeed, const float runSpeed):
    MapEntity(textureController, spriteFilepath, camera, fileReader, tilemap, Direction::Down, walkSpeed, runSpeed)
{
    SetMapPosition(position);
    const MapPosition mp = GetMapPosition();
    tilemap.TakePosition(mp); // Should be in MapEntity (currently not possible because spawn position is defined in NPC constructor)
    m_position = GetFinalDrawingPosition(mp.ToScenePosition(tilemap.GetTileSize()));
    m_behaviour = std::make_unique<RandomBehaviour>();
    //m_behaviour = std::make_unique<FollowEntityBehaviour>(trackedEntity, GetWalkSpeed());
    //m_behaviour = std::make_unique<GoToBehaviour>(GetMapPosition(), MapPosition{4, 1}, tilemap);
}

void NPC::Update(const float deltaTime)
{
    switch (GetState()){ // This code has the same structure than Player::Update, I think I can merge it in MapEntity::Update
        case EntityState::Free:
        {
            m_behaviour->FreeCase(*this, deltaTime);
            break;
        }

        case EntityState::Moving:
        {
            m_behaviour->MovingCase(*this, deltaTime);
            break;
        }

        case EntityState::OnStop: // Enter this case at the end of the current movement
        {
            Notify(EntityEvent::SortEntity); // Will sort the entities rendered by the Scene
            m_behaviour->OnStopCase(*this);
            break;
        }

        default: // Should not happen
            break; 
    }
}
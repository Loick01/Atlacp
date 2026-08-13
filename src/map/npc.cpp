#include "map/npc.hpp"

#include "tile/tilemap.hpp"

NPC::NPC(const FileReader& fileReader, Tilemap& tilemap, TextureController& textureController, const std::string& spriteFilepath,
    Camera& camera, const MapPosition position, const float walkSpeed, const float runSpeed, const unsigned int id):
    MapEntity(textureController, spriteFilepath, camera, fileReader, tilemap, Direction::Down, walkSpeed, runSpeed, id)
{
    SetMapPosition(position);
    const MapPosition mp = GetMapPosition();
    tilemap.TakePosition(mp); // Should be in MapEntity (currently not possible because spawn position is defined in NPC constructor)
    m_position = GetFinalDrawingPosition(mp.ToScenePosition(tilemap.GetTileSize()));
    m_behaviour = std::make_unique<MapRandomBehaviour>(); // NPC always spawn with random behaviour
}

void NPC::Update(const float deltaTime)
{
    // Should test if m_behaviour != nullptr ?
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

void NPC::SetBehaviour(const MapBehaviour mb)
{
    switch (mb) {
        case MapBehaviour::Random :
            m_behaviour = std::make_unique<MapRandomBehaviour>();
            break;
        // case MapBehaviour::Follow :
        //     m_behaviour = std::make_unique<MapFollowBehaviour>(this, trackedEntity); // const MapEntity* followerEntity, trackedEntity
        //     break;
        // case MapBehaviour::GoTo :
        //     m_behaviour = std::make_unique<MapGoToBehaviour>(GetMapPosition(), MapPosition{4, 1}, tilemap);
        //     break;
        default :
            throw std::runtime_error("Unknown MapBehaviour value");
    }
}
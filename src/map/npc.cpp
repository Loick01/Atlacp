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

const MapEntityBehaviour* NPC::GetMapBehaviour() const
{
    return m_behaviour.get();
}

MapEntityBehaviour* NPC::GetMapBehaviour()
{
    return m_behaviour.get();
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
            // Notify(EntityEvent::HasMoved); For now I disable triggers for NPCs
            m_behaviour->OnStopCase(*this);
            break;
        }

        default: // Should not happen
            break; 
    }
}

// Instead of SetBehaviour(), I think I will create one function for each behaviour, like below
void NPC::SetRandomBehaviour()
{
    m_behaviour = std::make_unique<MapRandomBehaviour>();
}

void NPC::SetGoToBehaviour(const Tilemap& tilemap, const MapPosition target)
{
    m_behaviour = std::make_unique<MapGoToBehaviour>(GetMapPosition(), target, tilemap);
}
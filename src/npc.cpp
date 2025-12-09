#include "npc.hpp"

NPC::NPC(const FileReader* file_reader, Tilemap* tilemap, TextureController* texture_controller,
    const std::string& sprite_filepath, Camera* camera, const float speed):
    Entity(texture_controller, sprite_filepath, camera, file_reader, tilemap, speed)
{
    SetMapPosition(MapPosition{16,22});
    m_position = GetFinalDrawingPosition(m_map_position.ToScenePosition(tilemap->GetTileSize()));
}

NPC::~NPC()
{

}

void NPC::Update(const float delta_time)
{
    switch (m_state){
        case ElementState::Free:
        {
            MapMovement movement;
            movement.DefineMovement(m_random.GetRandomDirection());
            StartMovement(movement, true);
            break;
        }

        case ElementState::Moving:
        {
            m_position = GetFinalDrawingPosition(ContinueMovement(delta_time));
            break;
        }

        case ElementState::StopMoving: // Enter this case at the end of the current movement
        {
            MapMovement movement;
            movement.DefineMovement(m_random.GetRandomDirection());
            StartMovement(movement, false);
            break;
        }

        default: // Should not happen
            break; 
    }
}
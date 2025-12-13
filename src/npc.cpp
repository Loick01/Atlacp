#include "npc.hpp"

NPC::NPC(const FileReader* file_reader, Tilemap* tilemap, TextureController* texture_controller,
    const std::string& sprite_filepath, Camera* camera, const float speed):
    Entity(texture_controller, sprite_filepath, camera, file_reader, tilemap, speed)
{
    SetMapPosition(MapPosition{16,22}); // Only for ff_world
    const MapPosition mp = GetMapPosition();
    tilemap->TakePosition(mp); // Should be in Entity ?
    m_position = GetFinalDrawingPosition(mp.ToScenePosition(tilemap->GetTileSize()));
}

NPC::~NPC()
{

}

void NPC::Update(const float delta_time)
{
    switch (GetState()){
        case EntityState::Free:
        {
            EntityMovement movement;
            movement.DefineMovement(m_random.GetRandomDirection());
            StartMovement(movement, true);
            break;
        }

        case EntityState::Moving:
        {
            m_position = GetFinalDrawingPosition(ContinueMovement(delta_time));
            break;
        }

        case EntityState::StopMoving: // Enter this case at the end of the current movement
        {
            EntityMovement movement;
            movement.DefineMovement(m_random.GetRandomDirection());
            StartMovement(movement, false);
            break;
        }

        default: // Should not happen
            break; 
    }
}
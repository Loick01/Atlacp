#include "npc.hpp"

NPC::NPC(const FileReader& file_reader, Tilemap& tilemap, TextureController& texture_controller, const Entity* tracked_entity, // Entity parameter use for follow behaviour, should not be here
    const std::string& sprite_filepath, Camera& camera, const float speed):
    Entity(texture_controller, sprite_filepath, camera, file_reader, tilemap, speed)
{
    SetMapPosition(MapPosition{16,22}); // Only for ff_world
    const MapPosition mp = GetMapPosition();
    tilemap.TakePosition(mp); // Should be in Entity (currently not possible because spawn position if defined in NPC constructor)
    m_position = GetFinalDrawingPosition(mp.ToScenePosition(tilemap.GetTileSize()));
    //m_behaviour = new RandomBehaviour();
    m_behaviour = new FollowEntityBehaviour(tracked_entity, GetSpeed());
    //m_behaviour = new GoToBehaviour(GetMapPosition(), MapPosition{4, 1}, tilemap);
}

void NPC::Update(const float delta_time)
{
    switch (GetState()){ // This code has the same structure than Player::Update, I think I can merge it in Entity::Update
        case EntityState::Free:
        {
            m_behaviour->FreeCase(*this);
            break;
        }

        case EntityState::Moving:
        {
            m_behaviour->MovingCase(*this, delta_time);
            break;
        }

        case EntityState::OnStop: // Enter this case at the end of the current movement
        {
            m_behaviour->OnStopCase(*this);
            break;
        }

        default: // Should not happen
            break; 
    }
}
#include "npc.hpp"

NPC::NPC(const FileReader* file_reader, Tilemap* tilemap, TextureController* texture_controller,
    const std::string& sprite_filepath, Camera* camera, const float speed):
    Entity(texture_controller, sprite_filepath, camera, file_reader, tilemap, speed)
{
    SetMapPosition(MapPosition{16,15});
    m_position = GetFinalDrawingPosition(m_map_position.ToScenePosition(tilemap->GetTileSize()));
}

NPC::~NPC()
{

}

void NPC::Update(const float delta_time)
{

}
#include "entity.hpp"

Entity::Entity(TextureController* texture_controller, const std::string& sprite_filepath, Camera* camera, const FileReader* file_reader,
    Tilemap* tilemap, const float speed):
    Drawable(texture_controller, sprite_filepath+".png", camera, ScenePosition{0,0}), MapElement(file_reader, sprite_filepath, tilemap, speed)
    // Remove +".png" if I create RessourceFile struct ?
{
    const Pair<int> sprite_size = m_animation.GetSpriteSize();
    m_texture_width = sprite_size.x;
    m_texture_height = sprite_size.y;
    m_display_offset = ScenePosition{(m_texture_width-tilemap->GetTileSize())/2, m_texture_height-tilemap->GetTileSize()};
}

Entity::~Entity()
{

}

ScenePosition Entity::GetFinalDrawingPosition(const ScenePosition sp) const
{
    return (sp-m_display_offset)*m_camera->GetZoom();
}

void Entity::DrawTexture() const
{
    const Pair<int> sprite = m_animation.GetCurrentSprite(); 
    const SDL_Rect src{sprite.x, sprite.y, m_texture_width, m_texture_height};
    const ScenePosition camera_position = m_camera->GetCameraPosition();
    const ScenePosition camera_offset = m_camera->GetOffset(); // Remove
    const float zoom = m_camera->GetZoom();
    const SDL_Rect dst{m_position.x-camera_position.x+camera_offset.x, m_position.y-camera_position.y+camera_offset.y,
                       static_cast<int>(m_texture_width*zoom), static_cast<int>(m_texture_height*zoom)};
    m_texture_controller->RenderTexture(m_texture_key, src, dst);
}
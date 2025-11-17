#include "drawable.hpp"

Drawable::Drawable(TextureController* texture_controller, const std::string& texture_filepath, Camera* camera, const ScenePosition position):
    m_texture_controller(texture_controller), m_position(position), m_camera(camera)
{
    LoadTexture(texture_filepath);
}

Drawable::Drawable(TextureController* texture_controller, Camera* camera, const ScenePosition position):
    m_texture_controller(texture_controller), m_position(position), m_camera(camera)
{
    // This constructor is used only for Tileset and Tilemap, maybe I can remove it
}

Drawable::~Drawable()
{
    
}

void Drawable::LoadTexture(const std::string& texture_filepath)
{
    m_texture_controller->DeleteTexture(m_texture_key);
    m_texture_key = texture_filepath; // Use hash function to get a key from the filepath (unless TextureKey is already std::string)
    m_texture_controller->LoadTextureFromFile(texture_filepath, m_texture_key, m_texture_width, m_texture_height); 
}

TextureKey Drawable::GetTextureKey() const
{
    return m_texture_key;
}

ScenePosition Drawable::GetScenePosition() const
{
    return m_position;
}

int Drawable::GetTextureWidth() const
{
    return m_texture_width;
}

int Drawable::GetTextureHeight() const
{
    return m_texture_height;
}

bool Drawable::IsPositionInTexture(const ScreenPosition sp) const // sp must be normalized (with screen position)
{
    // Use GetTextureWidth()/GetTextureHeight() instead of m_texture_width/m_texture_height because of overrided functions
    return sp.x >= 0 && sp.y >= 0 && sp.x <= GetTextureWidth() && sp.y <= GetTextureHeight();
}

bool Drawable::IsPositionInTexture(const ScenePosition sp) const // sp must be normalized (with scene position)
{
    // Use GetTextureWidth()/GetTextureHeight() instead of m_texture_width/m_texture_height because of overrided functions
    return sp.x >= 0 && sp.y >= 0 && sp.x <= GetTextureWidth() && sp.y <= GetTextureHeight();
}

void Drawable::DrawTexture() const
{
    const SDL_Rect src{0, 0, m_texture_width, m_texture_height};
    const ScenePosition camera_position = m_camera->GetCameraPosition();
    const SDL_Rect dst{m_position.x-camera_position.x, m_position.y-camera_position.y, m_texture_width, m_texture_height};
    m_texture_controller->RenderTexture(m_texture_key, src, dst);
}

void Drawable::SetScenePosition(const ScenePosition sp)
{
    m_position = sp;
}

void Drawable::AddScenePosition(const ScenePosition sp)
{
    m_position = m_position + sp;
}
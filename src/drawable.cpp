#include "drawable.hpp"

Drawable::Drawable(TextureController& texture_controller, const std::string& texture_filepath):
    m_texture_controller(texture_controller)
{
    LoadTexture(texture_filepath);
}

Drawable::Drawable(TextureController& texture_controller):
    m_texture_controller(texture_controller)
{

}

Drawable::~Drawable()
{
    m_texture_controller.DeleteTexture(m_texture_key);
}

void Drawable::LoadTexture(const std::string& texture_filepath)
{
    m_texture_controller.DeleteTexture(m_texture_key);
    m_texture_key = texture_filepath; // Use hash function to get a key from the filepath (unless TextureKey is already std::string)
    m_texture_controller.LoadTextureFromFile(texture_filepath, m_texture_key, m_texture_width, m_texture_height); 
}

TextureKey Drawable::GetTextureKey() const
{
    return m_texture_key;
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

SceneDrawable::SceneDrawable(TextureController& texture_controller, const std::string& texture_filepath, Camera& camera, const ScenePosition position):
    Drawable(texture_controller, texture_filepath), m_camera(camera), m_position(position)
{

}

SceneDrawable::SceneDrawable(TextureController& texture_controller, Camera& camera, const ScenePosition position):
    Drawable(texture_controller), m_camera(camera), m_position(position)
{

}

ScenePosition SceneDrawable::GetDisplayOffset() const
{
    return m_display_offset;
}   

void SceneDrawable::SetDisplayOffset(const ScenePosition offset)
{
    m_display_offset = offset;
}

void SceneDrawable::LookMe()
{
    m_camera.LookAt(m_position+m_display_offset);
}

ScreenDrawable::ScreenDrawable(TextureController& texture_controller, const std::string& texture_filepath, const ScreenPosition position, const bool should_draw):
    Drawable(texture_controller, texture_filepath), m_position(position), m_should_draw(should_draw), m_zoom(1.0f) // Default value for zoom must be 1.0
{
    
}

ScreenDrawable::ScreenDrawable(TextureController& texture_controller, const ScreenPosition position, const bool should_draw):
    Drawable(texture_controller), m_position(position), m_should_draw(should_draw), m_zoom(1.0f) // Default value for zoom must be 1.0
{

}

ScreenPosition ScreenDrawable::GetScreenPosition() const
{
    return m_position;
}

ScreenPosition ScreenDrawable::GetSize() const
{
    return {GetTextureWidth(), GetTextureHeight()};
}

float ScreenDrawable::GetZoom() const
{
    return m_zoom;
}

bool ScreenDrawable::GetShouldDraw() const
{
    return m_should_draw;
}

void ScreenDrawable::SetZoom(const float zoom)
{
    m_zoom = zoom;
}

void ScreenDrawable::DrawTexture() const
{
    if (m_should_draw){
        const SDL_Rect src{0, 0, m_texture_width, m_texture_height};
        const SDL_Rect dst{m_position.x, m_position.y, static_cast<int>(m_texture_width*m_zoom), static_cast<int>(m_texture_height*m_zoom)};
        m_texture_controller.RenderTexture(m_texture_key, src, dst);
    }
}

void ScreenDrawable::SetScreenPosition(const ScreenPosition position)
{
    m_position = position;
}

void ScreenDrawable::InvertShouldDraw()
{
    m_should_draw = !m_should_draw;
}
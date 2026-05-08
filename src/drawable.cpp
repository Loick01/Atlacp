#include "drawable.hpp"

Drawable::Drawable(TextureController& textureController, const std::string& textureFilepath):
    m_textureController(textureController)
{
    LoadTexture(textureFilepath);
}

Drawable::Drawable(TextureController& textureController):
    m_textureController(textureController)
{

}

Drawable::~Drawable()
{
    DeleteTexture();
}

void Drawable::LoadTexture(const std::string& textureFilepath)
{
    m_textureKey = textureFilepath; // Use hash function to get a key from the filepath (unless TextureKey is already std::string)
    m_textureController.LoadTextureFromFile(textureFilepath, m_textureKey, m_textureWidth, m_textureHeight); 
}

TextureKey Drawable::GetTextureKey() const
{
    return m_textureKey;
}

int Drawable::GetTextureWidth() const
{
    return m_textureWidth;
}

int Drawable::GetTextureHeight() const
{
    return m_textureHeight;
}

bool Drawable::IsPositionInTexture(const Vec2 sp) const // sp must be normalized
{
    return sp.x >= 0 && sp.y >= 0 && sp.x <= GetTextureWidth() && sp.y <= GetTextureHeight();
}

void Drawable::DeleteTexture()
{
    m_textureController.DeleteTexture(m_textureKey);
}

SceneDrawable::SceneDrawable(TextureController& textureController, const std::string& textureFilepath, Camera& camera, const ScenePosition position):
    Drawable(textureController, textureFilepath), m_camera(camera), m_position(position)
{}

ScenePosition SceneDrawable::GetDisplayOffset() const
{
    return m_displayOffset;
}   

void SceneDrawable::SetDisplayOffset(const ScenePosition offset)
{
    m_displayOffset = offset;
}

void SceneDrawable::LookMe()
{
    m_camera.LookAt(m_position+m_displayOffset);
}

ScreenDrawable::ScreenDrawable(TextureController& textureController, const std::string& textureFilepath, const ScreenPosition position, const bool shouldDraw):
    Drawable(textureController, textureFilepath), m_position(position), m_shouldDraw(shouldDraw), m_zoom(1.0f) // Default value for zoom must be 1.0
{}

ScreenDrawable::ScreenDrawable(TextureController& textureController, const ScreenPosition position, const bool shouldDraw):
    Drawable(textureController), m_position(position), m_shouldDraw(shouldDraw), m_zoom(1.0f) // Default value for zoom must be 1.0
{}

ScreenPosition ScreenDrawable::GetScreenPosition() const
{
    return m_position;
}

ScreenPosition ScreenDrawable::GetSize() const
{
    return ScreenPosition{GetTextureWidth(), GetTextureHeight()}*m_zoom;
}

float ScreenDrawable::GetZoom() const
{
    return m_zoom;
}

bool ScreenDrawable::GetShouldDraw() const
{
    return m_shouldDraw;
}

void ScreenDrawable::SetZoom(const float zoom)
{
    m_zoom = zoom;
}

void ScreenDrawable::DrawTexture() const
{
    if (m_shouldDraw){
        const SDL_Rect src{0, 0, m_textureWidth, m_textureHeight};
        const SDL_Rect dst{m_position.x, m_position.y, static_cast<int>(m_textureWidth*m_zoom), static_cast<int>(m_textureHeight*m_zoom)};
        m_textureController.RenderTexture(m_textureKey, src, dst);
    }
}

void ScreenDrawable::SetScreenPosition(const ScreenPosition position)
{
    m_position = position;
}

void ScreenDrawable::InvertShouldDraw()
{
    m_shouldDraw = !m_shouldDraw;
}
#include "drawable.hpp"

Drawable::Drawable(TextureController* texture_controller, const std::string& texture_filepath, const ScreenPosition position, const bool should_draw) :
    m_texture_controller(texture_controller), m_screen_position(position), m_should_draw(should_draw)
{
    LoadTexture(texture_filepath);
}

Drawable::~Drawable()
{
    m_texture_controller->DeleteTexture(m_texture_key);
}

void Drawable::LoadTexture(const std::string& texture_filepath)
{
    m_texture_key = texture_filepath; // Use hash function to get a key from the filepath (unless TextureKey is already std::string)
    m_texture_controller->DeleteTexture(m_texture_key);
    m_texture_controller->LoadTextureFromFile(texture_filepath, m_texture_key, m_texture_width, m_texture_height); 
}

ScreenPosition Drawable::GetScreenPosition() const
{
    return m_screen_position;
}

unsigned int Drawable::GetTextureWidth() const
{
    return m_texture_width;
}

unsigned int Drawable::GetTextureHeight() const
{
    return m_texture_height;
}

bool Drawable::GetShouldDraw() const
{
    return m_should_draw;
}

bool Drawable::IsPositionInTexture(const ScreenPosition screen_position) const // screen_position must be normalized
{
    // Use GetTextureWidth()/GetTextureHeight() instead of m_texture_width/m_texture_height
    return screen_position.x >= 0 && screen_position.y >= 0 && screen_position.x <= GetTextureWidth() && screen_position.y <= GetTextureHeight();
}

void Drawable::DrawTexture() const
{
    if (m_should_draw){
        int texture_width = static_cast<int>(m_texture_width);
        int texture_height = static_cast<int>(m_texture_height);
        const SDL_Rect src{0, 0, texture_width, texture_height};
        const SDL_Rect dst{m_screen_position.x, m_screen_position.y, texture_width, texture_height};
        m_texture_controller->RenderTexture(m_texture_key, src, dst);
    }
}

void Drawable::SetScreenPosition(const ScreenPosition screen_position)
{
    m_screen_position = screen_position;
}

void Drawable::InvertShouldDraw()
{
    m_should_draw = !m_should_draw;
}
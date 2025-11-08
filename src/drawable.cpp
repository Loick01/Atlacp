#include "drawable.hpp"

Drawable::Drawable(TextureController* texture_controller, const std::string& texture_filepath, const ScreenPosition position) :
    m_texture_controller(texture_controller), m_screen_position(position)
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

void Drawable::SetScreenPosition(const ScreenPosition screen_position)
{
    m_screen_position = screen_position;
}

void Drawable::DrawTexture() const
{
    int texture_width = static_cast<int>(m_texture_width);
    int texture_height = static_cast<int>(m_texture_height);
    const SDL_Rect src{0, 0, texture_width, texture_height};
    const SDL_Rect dst{m_screen_position.x, m_screen_position.y, texture_width, texture_height};
    m_texture_controller->RenderTexture(m_texture_key, src, dst);
}
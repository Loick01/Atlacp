#include "drawable.hpp"

Drawable::Drawable(TextureController* texture_controller, const std::string& texture_filepath) :
    m_texture_controller(texture_controller), m_texture_filepath(texture_filepath)
{
    m_texture_controller->LoadTextureFromFile(texture_filepath); // Only for constructor, then use LoadTexture
}

Drawable::~Drawable()
{
    m_texture_controller->DeleteTexture(m_texture_filepath);
}

void Drawable::LoadTexture(const std::string& texture_filepath)
{
    m_texture_controller->DeleteTexture(m_texture_filepath);
    m_texture_filepath = texture_filepath;
    m_texture_controller->LoadTextureFromFile(texture_filepath); 
}
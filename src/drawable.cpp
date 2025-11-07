#include "drawable.hpp"

Drawable::Drawable(TextureController* texture_controller, const std::string& texture_filepath, const Offset offset) :
    m_texture_controller(texture_controller), m_offset(offset)
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
    m_texture_controller->LoadTextureFromFile(texture_filepath, m_texture_key); 
}

void Drawable::SetOffset(const Offset offset)
{
    m_offset = offset;
}
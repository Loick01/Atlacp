#include "texture.hpp"

TextureController::TextureController(SDL_Renderer* window_renderer) :
    m_window_renderer(window_renderer)
{

}

TextureController::~TextureController()
{
    std::map<std::string, SDL_Texture*>::iterator it;
    for (it = m_textures.begin() ; it != m_textures.end() ; it++){
        SDL_DestroyTexture(it->second);
    }
}

void TextureController::LoadTextureFromFile(const std::string& filepath)
{
    SDL_Surface* surface = IMG_Load(filepath.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_window_renderer,surface);
    if (!surface) std::cout << "Failed to load this texture : " << filepath << "\n";
    else if (!texture) std::cout << "Failed to convert this surface into a texture : " << filepath << "\n";
    SDL_FreeSurface(surface);
    if (m_textures[filepath]) SDL_DestroyTexture(m_textures[filepath]);
    m_textures[filepath] = texture;
}

void TextureController::RenderTexture(const std::string& texture_name, const SDL_Rect& src, const SDL_Rect& dst) const
{
    // A texture with key=texture_name must already be in the map, otherwise std::out_of_range
    SDL_RenderCopy(m_window_renderer, m_textures.at(texture_name), &src, &dst);
}

void TextureController::DeleteTexture(const std::string& texture_name)
{
    std::map<std::string, SDL_Texture*>::iterator it = m_textures.find(texture_name);
    if (it != m_textures.end()){
        SDL_DestroyTexture(it->second);
        m_textures.erase(it);
    }
    else std::cout << "Can't delete " << texture_name << ", it's not in the map (this should not happen)\n";
}
#include "texture.hpp"

TextureController::TextureController(SDL_Renderer* window_renderer) :
    m_window_renderer(window_renderer)
{

}

TextureController::~TextureController()
{
    for (const std::pair<const TextureKey, SDL_Texture*>& p : m_textures){
        std::cout << "Drawable should destroy their respective texture in their own destructor. Try to avoid being here\n";
        SDL_DestroyTexture(p.second);
    }
}

void TextureController::LoadTextureFromFile(const std::string& texture_filepath, const TextureKey& texture_key, int& texture_width, int& texture_height)
{
    SDL_Surface* surface = IMG_Load(texture_filepath.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_window_renderer,surface);
    if (!surface) std::cout << "Failed to load this texture : " << texture_filepath << "\n";
    else if (!texture) std::cout << "Failed to convert this surface into a texture : " << texture_filepath << "\n";
    texture_width = surface->w, texture_height = surface->h;
    SDL_FreeSurface(surface);
    if (m_textures[texture_key]) SDL_DestroyTexture(m_textures[texture_key]);
    m_textures[texture_key] = texture;
}

void TextureController::RenderTexture(const TextureKey& texture_key, const SDL_Rect& src, const SDL_Rect& dst) const
{
    // A texture with key=texture_key must already be in the map, otherwise std::out_of_range
    SDL_RenderCopy(m_window_renderer, m_textures.at(texture_key), &src, &dst);
}

void TextureController::DeleteTexture(const TextureKey& texture_key)
{
    std::map<TextureKey, SDL_Texture*>::iterator it = m_textures.find(texture_key);
    if (it != m_textures.end()){
        SDL_DestroyTexture(it->second);
        m_textures.erase(it);
    }
}
#include "texture.hpp"

std::map<std::string,SDL_Texture*> TextureController::m_textures;

TextureController::TextureController(SDL_Renderer* window_renderer) :
    m_window_renderer(window_renderer)
{
    LoadTextureFromFile("../cpp.png");
}

TextureController::~TextureController()
{
    std::map<std::string, SDL_Texture*>::iterator it;
    for (it = m_textures.begin() ; it != m_textures.end() ; it++){
        SDL_DestroyTexture(it->second);
    }
}

void TextureController::LoadTextureFromFile(const char* filepath)
{
    SDL_Surface* surface = IMG_Load(filepath);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_window_renderer,surface);
    if (!surface) std::cout << "Failed to load this texture : " << filepath << "\n";
    else if (!texture) std::cout << "Failed to convert this surface into a texture : " << filepath << "\n";
    SDL_FreeSurface(surface);
    if (m_textures[filepath]) SDL_DestroyTexture(m_textures[filepath]);
    m_textures[filepath] = texture;
}

void TextureController::DeleteTexture(const char* texture_name)
{
    std::map<std::string, SDL_Texture*>::iterator it = m_textures.find(texture_name);
    if (it != m_textures.end()){
        SDL_DestroyTexture(it->second);
        m_textures.erase(it);
    }
    else std::cout << "Can't delete " << texture_name << ", not in the map\n";
}

void TextureController::RenderTexture(const char* texture_name, const SDL_Rect src, const SDL_Rect dst)
{
    SDL_RenderCopy(m_window_renderer, m_textures[texture_name], &src, &dst);
}
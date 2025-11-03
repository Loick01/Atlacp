#include "texture.hpp"

std::map<std::string,SDL_Surface*> TextureController::m_textures;

TextureController::TextureController()
{
    LoadTextureFromFile("../cpp.bmp");
}

TextureController::~TextureController()
{
    std::map<std::string, SDL_Surface*>::iterator it;
    for (it = m_textures.begin() ; it != m_textures.end() ; it++){
        // std::cout << "Delete texture : " << it->first << "\n";
        SDL_FreeSurface(it->second);
    }
}

void TextureController::LoadTextureFromFile(const char* filepath)
{
    SDL_Surface* s = SDL_LoadBMP(filepath);
    if (!s) std::cout << "Failed to load this texture : " << filepath << "\n";
    m_textures[filepath] = s;
}

void TextureController::DeleteTexture(const char* texture_name)
{
    std::map<std::string, SDL_Surface*>::iterator it = m_textures.find(texture_name);
    if (it != m_textures.end()){
        SDL_FreeSurface(it->second);
        m_textures.erase(it);
    }
    else std::cout << "Can't delete " << texture_name << ", not in the map\n";
}

void TextureController::DrawTexture(const char* texture_name, SDL_Surface* window_surface, SDL_Rect position)
{
    SDL_BlitSurface(m_textures[texture_name], NULL, window_surface, &position);
}
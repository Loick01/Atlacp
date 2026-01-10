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
    for (const std::pair<const TextureKey, TTF_Font*>& f : m_fonts){
        // For now, I haven't implemented a counter to know how many textures use a font. All the fonts are closed here
        std::cout << "TTF_Font should be destroyed by the last TextArea that uses it. Try to avoid being here\n";
        TTF_CloseFont(f.second);
    }
}

TTF_Font* TextureController::GetFont(const TextureKey& texture_key) const
{
    return m_fonts.at(texture_key); // No verifications for the moment
}

void TextureController::LoadImageFromFile(const std::string& texture_filepath, const TextureKey& texture_key, int& texture_width, int& texture_height)
{
    if (m_textures.find(texture_key) == m_textures.end()){
        SDL_Surface* surface = IMG_Load(texture_filepath.c_str());
        if (!surface) std::cout << "Failed to load this texture : " << texture_filepath << "\n";
        SDL_Texture* texture = SDL_CreateTextureFromSurface(m_window_renderer,surface);
        if (!texture) std::cout << "Failed to convert this surface into a texture : " << texture_filepath << "\n";
        texture_width = surface->w, texture_height = surface->h;
        SDL_FreeSurface(surface);
        m_textures[texture_key] = texture;
    }else{ // This file has already been loaded as a texture (SDL_Texture already exists in m_textures)
        SDL_QueryTexture(m_textures[texture_key], nullptr, nullptr, &texture_width, &texture_height); // LoadImageFromFile could return texture size instead of using extra parameters ?
    }
}

void TextureController::LoadFontFromFile(const std::string& font_filepath, const TextureKey& texture_key, const int font_size)
{
    if (m_fonts.find(texture_key) == m_fonts.end()){
        TTF_Font* font = TTF_OpenFont(font_filepath.c_str(), font_size);
        if (!font) std::cout << "Failed to load this font : " << font_filepath << "\n";
        m_fonts[texture_key] = font;
    }/*else{ // Will be removed (different TextArea could use the same font)
        // std::cout << "This font has already been loaded as a font : " << texture_key << "\n";
    }*/
}

void TextureController::RenderTexture(const TextureKey& texture_key, const SDL_Rect& src, const SDL_Rect& dst) const
{
    // A texture with key=texture_key must already be in the map, otherwise std::out_of_range
    SDL_RenderCopy(m_window_renderer, m_textures.at(texture_key), &src, &dst);
}

void TextureController::DeleteTexture(const TextureKey& texture_key)
{
    // Same as for the text fonts, I should implement a counter to know how many Drawable use a texture (they could be shared)
    std::map<TextureKey, SDL_Texture*>::iterator it = m_textures.find(texture_key);
    if (it != m_textures.end()){
        SDL_DestroyTexture(it->second);
        m_textures.erase(it);
    }/*else{
        std::cout << "Try to delete a texture that don't exist : " << texture_key << "\n"; // Will throw an error (is this case possible ?)
    }*/
}

// void TextureController::DeleteFont(const TextureKey& texture_key)
// {
//     std::map<TextureKey, TTF_Font*>::iterator it = m_fonts.find(texture_key);
//     if (it != m_fonts.end()){
//         TTF_CloseFont(it->second);
//         m_fonts.erase(it);
//     }
// }

void TextureController::LoadTextureFromText(const TextureKey& font_key, const TextureKey& texture_key, const std::string& text, 
    int &texture_width, int& texture_height, const SDL_Color text_color, const int max_width)
{
    SDL_Surface* surface = TTF_RenderUTF8_Blended_Wrapped(GetFont(font_key), text.c_str(), text_color, max_width);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_window_renderer, surface);
    texture_width = surface->w, texture_height = surface->h;
    SDL_FreeSurface(surface);
    if (m_textures[texture_key]) SDL_DestroyTexture(m_textures[texture_key]);
    m_textures[texture_key] = texture;
}
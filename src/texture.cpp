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
        std::cout << "TextArea should destroy their respective font in their own destructor. Try to avoid being here\n";
        TTF_CloseFont(f.second);
    }
}

TTF_Font* TextureController::GetFont(const TextureKey& texture_key) const
{
    return m_fonts.at(texture_key); // No verifications for the moment
}

void TextureController::LoadTextureFromFile(const std::string& texture_filepath, const TextureKey& texture_key, int& texture_width, int& texture_height)
{
    SDL_Surface* surface = IMG_Load(texture_filepath.c_str());
    if (!surface) std::cout << "Failed to load this texture : " << texture_filepath << "\n";
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_window_renderer,surface);
    if (!texture) std::cout << "Failed to convert this surface into a texture : " << texture_filepath << "\n";
    texture_width = surface->w, texture_height = surface->h;
    SDL_FreeSurface(surface);
    if (m_textures[texture_key]) SDL_DestroyTexture(m_textures[texture_key]);
    m_textures[texture_key] = texture;
}

void TextureController::LoadFontFromFile(const std::string& font_filepath, const TextureKey& texture_key, const int font_size)
{
    TTF_Font* font = TTF_OpenFont(font_filepath.c_str(), font_size);
    if (!font) std::cout << "Failed to load this font : " << font_filepath << "\n";
    if (m_fonts[texture_key]) TTF_CloseFont(m_fonts[texture_key]);
    m_fonts[texture_key] = font;
}

void TextureController::RenderTexture(const TextureKey& texture_key, const SDL_Rect& src, const SDL_Rect& dst) const
{
    // A texture with key=texture_key must already be in the map, otherwise std::out_of_range
    SDL_RenderCopy(m_window_renderer, m_textures.at(texture_key), &src, &dst);
}

// Will be deleted
void TextureController::RenderFont(SDL_Texture* texture, const SDL_Rect& dst) const
{
    SDL_RenderCopy(m_window_renderer, texture, nullptr, &dst);
}

void TextureController::DeleteTexture(const TextureKey& texture_key)
{
    std::map<TextureKey, SDL_Texture*>::iterator it = m_textures.find(texture_key);
    if (it != m_textures.end()){
        SDL_DestroyTexture(it->second);
        m_textures.erase(it);
    }
}

void TextureController::DeleteFont(const TextureKey& texture_key)
{
    std::map<TextureKey, TTF_Font*>::iterator it = m_fonts.find(texture_key);
    if (it != m_fonts.end()){
        TTF_CloseFont(it->second);
        m_fonts.erase(it);
    }
}

SDL_Texture* TextureController::BuildTextureFromText(const TextureKey& font_key, const std::string& text, Pair<int>& area_size, const SDL_Color text_color) const
{
    SDL_Surface* surface = TTF_RenderUTF8_Blended(GetFont(font_key), text.c_str(), text_color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_window_renderer, surface);
    area_size.x = surface->w;
    area_size.y = surface->h;
    std::cout << "Text area size : " << area_size;
    SDL_FreeSurface(surface);
    return texture;
}
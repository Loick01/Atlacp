#include "texture.hpp"

TextureController::TextureController(SDL_Renderer* windowRenderer) :
    m_windowRenderer(windowRenderer)
{
    // SDL_image initialization
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags)
        throw std::runtime_error("Failed to initialize SDL image library\n" + std::string(IMG_GetError()));
    if (TTF_Init() < 0) 
        throw std::runtime_error("Failed to initialize SDL font library\n" + std::string(TTF_GetError()));
}

TextureController::~TextureController()
{
    for (const std::pair<const TextureKey, Texture>& p : m_textures){
        std::cout << "Should not be here --> Deleting " << p.first << " in ~TextureController\n"; // Will be removed
        SDL_DestroyTexture(p.second.texture);
    }
    for (const std::pair<const TextureKey, Font>& f : m_fonts){
        // For now, I haven't implemented a counter to know how many textures use a font. All the fonts are closed here
        // std::cout << "TTF_Font should be destroyed by the last TextArea that uses it. Try to avoid being here\n"; // Will be removed
        TTF_CloseFont(f.second.font);
    }
    IMG_Quit();
    TTF_Quit();
}

TTF_Font* TextureController::GetFont(const TextureKey& textureKey) const
{
    return m_fonts.at(textureKey).font; // No verifications for the moment
}

void TextureController::LoadTextureFromFile(const std::string& textureFilepath, const TextureKey& textureKey, int& textureWidth, int& textureHeight)
{
    if (m_textures.find(textureKey) == m_textures.end()){
        SDL_Surface* surface = IMG_Load(textureFilepath.c_str());
        if (!surface) 
            throw std::runtime_error("Failed to load this texture : " + textureFilepath + "\n" + std::string(IMG_GetError()));
        SDL_Texture* texture = SDL_CreateTextureFromSurface(m_windowRenderer,surface);
        if (!texture)
            throw std::runtime_error("Failed to convert this surface into a texture : " + textureFilepath + "\n" + std::string(SDL_GetError()));
        textureWidth = surface->w, textureHeight = surface->h;
        SDL_FreeSurface(surface);
        m_textures[textureKey].count = 1;
        m_textures[textureKey].texture = texture;
    }else{ // This file has already been loaded as a texture (SDL_Texture already exists in m_textures)
        m_textures[textureKey].count++;
        SDL_QueryTexture(m_textures[textureKey].texture, nullptr, nullptr, &textureWidth, &textureHeight); // LoadTextureFromFile could return texture size instead of using extra parameters ?
    }
}

void TextureController::LoadTextureFromText(const TextureKey& fontKey, const TextureKey& textureKey, const std::string& text, 
    int &textureWidth, int& textureHeight, const SDL_Color textColor, const int maxWidth)
{
    // Maybe I will change textureKey type
    if (textureKey == "") throw std::runtime_error("Texture key (string) is empty, text in TextArea should never be \"\"");
    
    if (m_textures.find(textureKey) == m_textures.end()) {
        SDL_Surface* surface = TTF_RenderUTF8_Blended_Wrapped(GetFont(fontKey), text.c_str(), textColor, maxWidth);
        if (!surface) 
            throw std::runtime_error("Failed to create a surface for this text : " + text + "\n" + std::string(IMG_GetError()));
        SDL_Texture* texture = SDL_CreateTextureFromSurface(m_windowRenderer, surface);
        if (!texture)
            throw std::runtime_error("Failed to convert this surface into a texture : " + text + "\n" + std::string(SDL_GetError()));
        textureWidth = surface->w, textureHeight = surface->h;
        SDL_FreeSurface(surface);
        m_textures[textureKey].count = 1;
        m_textures[textureKey].texture = texture;
    }else{ // This text has already been generated as a texture
        m_textures[textureKey].count++;
        SDL_QueryTexture(m_textures[textureKey].texture, nullptr, nullptr, &textureWidth, &textureHeight);
    }
}

void TextureController::LoadFontFromFile(const std::string& fontFilepath, const TextureKey& textureKey, const int fontSize)
{
    if (m_fonts.find(textureKey) == m_fonts.end()){
        TTF_Font* font = TTF_OpenFont(fontFilepath.c_str(), fontSize);
        if (!font) 
            throw std::runtime_error("Failed to load this font : " + fontFilepath + "\n" + std::string(TTF_GetError()));

        m_fonts[textureKey].count = 1;
        m_fonts[textureKey].font = font;
    } else {
        m_fonts[textureKey].count++;
    }
}

void TextureController::RenderTexture(const TextureKey& textureKey, const SDL_Rect& src, const SDL_Rect& dst) const
{
    // A texture with key=textureKey must already be in the map, otherwise std::out_of_range
    SDL_RenderCopy(m_windowRenderer, m_textures.at(textureKey).texture, &src, &dst);
}

void TextureController::DeleteTexture(const TextureKey& textureKey)
{
    std::map<TextureKey, Texture>::iterator it = m_textures.find(textureKey);
    if (it != m_textures.end()){
        if (it->second.count == 1) {
            SDL_DestroyTexture(it->second.texture);
            m_textures.erase(it);
        } else {
            it->second.count--;
        }
    } else {
        std::cout << "Try to delete a texture not in m_textures : " << textureKey << "\n";
        // throw std::runtime_error("Try to delete a texture that don't exist in TextureController : " + textureKey);
    }
}

// void TextureController::DeleteFont(const TextureKey& textureKey)
// {
//     std::map<TextureKey, TTF_Font*>::iterator it = m_fonts.find(textureKey);
//     if (it != m_fonts.end()){
//         TTF_CloseFont(it->second);
//         m_fonts.erase(it);
//     }
// }
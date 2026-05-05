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
    for (const std::pair<const TextureKey, SDL_Texture*>& p : m_textures){
        // std::cout << "Drawable should destroy their respective texture in their own destructor. Try to avoid being here\n";
        SDL_DestroyTexture(p.second);
    }
    for (const std::pair<const TextureKey, TTF_Font*>& f : m_fonts){
        // For now, I haven't implemented a counter to know how many textures use a font. All the fonts are closed here
        // std::cout << "TTF_Font should be destroyed by the last TextArea that uses it. Try to avoid being here\n";
        TTF_CloseFont(f.second);
    }
    IMG_Quit();
    TTF_Quit();
}

TTF_Font* TextureController::GetFont(const TextureKey& textureKey) const
{
    return m_fonts.at(textureKey); // No verifications for the moment
}

void TextureController::LoadImageFromFile(const std::string& textureFilepath, const TextureKey& textureKey, int& textureWidth, int& textureHeight)
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
        m_textures[textureKey] = texture;
    }else{ // This file has already been loaded as a texture (SDL_Texture already exists in m_textures)
        SDL_QueryTexture(m_textures[textureKey], nullptr, nullptr, &textureWidth, &textureHeight); // LoadImageFromFile could return texture size instead of using extra parameters ?
    }
}

void TextureController::LoadFontFromFile(const std::string& fontFilepath, const TextureKey& textureKey, const int fontSize)
{
    if (m_fonts.find(textureKey) == m_fonts.end()){
        TTF_Font* font = TTF_OpenFont(fontFilepath.c_str(), fontSize);
        if (!font) 
            throw std::runtime_error("Failed to load this font : " + fontFilepath + "\n" + std::string(TTF_GetError()));
        m_fonts[textureKey] = font;
    }
}

void TextureController::RenderTexture(const TextureKey& textureKey, const SDL_Rect& src, const SDL_Rect& dst) const
{
    // A texture with key=textureKey must already be in the map, otherwise std::out_of_range
    SDL_RenderCopy(m_windowRenderer, m_textures.at(textureKey), &src, &dst);
}

void TextureController::DeleteTexture(const TextureKey& textureKey)
{
    // Same as for the text fonts, I should implement a counter to know how many Drawable use a texture (they could be shared)
    std::map<TextureKey, SDL_Texture*>::iterator it = m_textures.find(textureKey);
    if (it != m_textures.end()){
        SDL_DestroyTexture(it->second);
        m_textures.erase(it);
    }/*else{
        std::cout << "Try to delete a texture that don't exist : " << textureKey << "\n"; // Will throw an error
    }*/
}

// void TextureController::DeleteFont(const TextureKey& textureKey)
// {
//     std::map<TextureKey, TTF_Font*>::iterator it = m_fonts.find(textureKey);
//     if (it != m_fonts.end()){
//         TTF_CloseFont(it->second);
//         m_fonts.erase(it);
//     }
// }

void TextureController::LoadTextureFromText(const TextureKey& fontKey, const TextureKey& textureKey, const std::string& text, 
    int &textureWidth, int& textureHeight, const SDL_Color textColor, const int maxWidth)
{
    // Maybe I will change textureKey type
    if (textureKey == "") throw std::runtime_error("Texture key (string) is empty, text in TextArea should never be \"\"");
    
    SDL_Surface* surface = TTF_RenderUTF8_Blended_Wrapped(GetFont(fontKey), text.c_str(), textColor, maxWidth);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_windowRenderer, surface);
    textureWidth = surface->w, textureHeight = surface->h;
    SDL_FreeSurface(surface);
    if (m_textures[textureKey]) SDL_DestroyTexture(m_textures[textureKey]);
    m_textures[textureKey] = texture;
}
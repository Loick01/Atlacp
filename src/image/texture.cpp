#include "image/texture.hpp"

#include "image/font.hpp"

TextureController::TextureController(FontController& fontController, SDL_Renderer* windowRenderer) :
    m_fontController(fontController), m_windowRenderer(windowRenderer)
{
    // SDL_image initialization
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags)
        Logger::GetInstance().AddLog(Level::L4, "Failed to initialize SDL_image: " + std::string(IMG_GetError()));
}

TextureController::~TextureController()
{
    for (const std::pair<const TextureKey, Texture>& p : m_textures){
        std::cout << "Should not be here --> Deleting " << p.first << " in ~TextureController\n"; // TODO : Will be removed and use Logger instead
        SDL_DestroyTexture(p.second.texture);
    }
    IMG_Quit();
}

void TextureController::CreateTextureFromSurface(SDL_Surface* surface, const TextureKey& key, const std::string& s, int& textureWidth, int& textureHeight)
{
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_windowRenderer, surface);
    if (!texture) {
        Logger::GetInstance().AddLog(Level::L4, "Failed to convert the surface " + s + " into a texture: " + std::string(SDL_GetError()));
        return;
    }

    textureWidth = surface->w, textureHeight = surface->h;
    SDL_FreeSurface(surface);
    m_textures[key].count = 1;
    m_textures[key].texture = texture;
}

void TextureController::AddTexture(const TextureKey& key, int& textureWidth, int& textureHeight)
{
    m_textures[key].count++;
    SDL_QueryTexture(m_textures[key].texture, nullptr, nullptr, &textureWidth, &textureHeight);
}

void TextureController::LoadTextureFromFile(const std::string& textureFilepath, const TextureKey& key, int& textureWidth, int& textureHeight)
{
    if (m_textures.find(key) == m_textures.end()){
        SDL_Surface* surface = IMG_Load(textureFilepath.c_str());
        if (!surface) 
            Logger::GetInstance().AddLog(Level::L4, "Failed to create the surface from: " + textureFilepath + " : " + std::string(IMG_GetError()));
        else
            CreateTextureFromSurface(surface, key, textureFilepath, textureWidth, textureHeight);
    } else { // Filepath has already been loaded as a texture
        AddTexture(key, textureWidth, textureHeight);
    }
}

void TextureController::LoadTextureFromText(const FontSize fontSize, const TextureKey& key, const std::string& text, 
    int &textureWidth, int& textureHeight, const SDL_Color textColor, const int maxWidth)
{
    if (key.empty()) {
        Logger::GetInstance().AddLog(Level::L4, "Texture key (string) is empty, text in UiTextElement should never be \"\"");
        return;
    }
    
    if (m_textures.find(key) == m_textures.end()) {
        SDL_Surface* surface = m_fontController.GenerateSurfaceFromText(fontSize, text, textColor, maxWidth);
        CreateTextureFromSurface(surface, key, text, textureWidth, textureHeight);
    }else{ // Text has already been generated as a texture
        AddTexture(key, textureWidth, textureHeight);
    }
}

void TextureController::RenderTexture(const TextureKey& key, const SDL_Rect& src, const SDL_Rect& dst) const
{
    // TODO : A texture with m_key=key must already be in the map, otherwise std::out_of_range
    // I will use Logger to check this
    SDL_RenderCopy(m_windowRenderer, m_textures.at(key).texture, &src, &dst);
}

void TextureController::DeleteTexture(const TextureKey& key)
{
    std::map<TextureKey, Texture>::iterator it = m_textures.find(key);
    if (it != m_textures.end()){
        if (it->second.count == 1) {
            SDL_DestroyTexture(it->second.texture);
            m_textures.erase(it);
        } else {
            it->second.count--;
        }
    } else {
        Logger::GetInstance().AddLog(Level::L4, "Try to delete a texture not in m_textures: " + key);
    }
}
#include "image/texture.hpp"

#include "image/font.hpp"

TextureController::TextureController(FontController& fontController, SDL_Renderer* windowRenderer) :
    m_fontController(fontController), m_windowRenderer(windowRenderer)
{
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags)
        Logger::GetInstance().AddLog(Level::L4, "Failed to initialize SDL_image: " + std::string(IMG_GetError())); // TODO : Level
}

TextureController::~TextureController()
{
    for (const std::pair<const TextureKey, Texture>& p : m_textures){
        Logger::GetInstance().AddLog(Level::L4, "Should not be here --> Deleting " + p.first + " in ~TextureController"); // TODO : Level
        SDL_DestroyTexture(p.second.texture);
    }
    IMG_Quit();
}

void TextureController::CreateTextureFromSurface(SDL_Surface* surface, const TextureKey& key, const std::string& s, int& textureWidth, int& textureHeight, const bool onFailure)
{
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_windowRenderer, surface);
    if (!texture) {
        Logger::GetInstance().AddLog(Level::L4, "Failed to convert the surface " + s + " into a texture: " + std::string(SDL_GetError())); // TODO : Level
        if (!onFailure) {
            AreaSize textureSize;
            LoadTextureFromFile("../assets/default_placeholder.png", key, textureSize, false);
            textureWidth = textureSize.x;
            textureHeight = textureSize.y;
        }
        return;
    }

    textureWidth = surface->w, textureHeight = surface->h;
    SDL_FreeSurface(surface);
    m_textures[key].count = 1;
    m_textures[key].texture = texture;
}

void TextureController::UseTexture(const TextureKey& key, int& textureWidth, int& textureHeight)
{
    m_textures[key].count++;
    SDL_QueryTexture(m_textures[key].texture, nullptr, nullptr, &textureWidth, &textureHeight);
}

void TextureController::LoadTextureFromFile(const std::string& textureFilepath, const TextureKey& key, AreaSize& textureSize, const bool onFailure)
{
    if (m_textures.find(key) == m_textures.end()){
        SDL_Surface* surface = IMG_Load(textureFilepath.c_str());
        if (surface) {
            CreateTextureFromSurface(surface, key, textureFilepath, textureSize.x, textureSize.y, onFailure);
        } else {
            Logger::GetInstance().AddLog(Level::L4, "Failed to create the surface from: " + textureFilepath + " : " + std::string(IMG_GetError())); // TODO : Level
            if (!onFailure) 
                LoadTextureFromFile("../assets/default_placeholder.png", key, textureSize, false);
        }
    } else {
        UseTexture(key, textureSize.x, textureSize.y);
    }
}

void TextureController::LoadTextureFromText(const FontSize fontSize, const TextureKey& key, const std::string& text, 
    AreaSize& textureSize, const SDL_Color textColor, const int maxWidth)
{
    if (text.empty()) {
        Logger::GetInstance().AddLog(Level::L4, "Text in UiTextElement should never be empty"); // TODO : Level
        return;
    }
    
    if (m_textures.find(key) == m_textures.end()) {
        SDL_Surface* surface = m_fontController.GenerateSurfaceFromText(fontSize, text, textColor, maxWidth);
        if (surface) {
            CreateTextureFromSurface(surface, key, text, textureSize.x, textureSize.y);
        } else {
            Logger::GetInstance().AddLog(Level::L4, "Failed to create the text surface for: " + key + " : " + std::string(IMG_GetError())); // TODO : Level
            LoadTextureFromFile("../assets/default_placeholder.png", key, textureSize, false);
        }
    } else {
        UseTexture(key, textureSize.x, textureSize.y);
    }
}

void TextureController::RenderTexture(const TextureKey& key, const SDL_Rect& src, const SDL_Rect& dst) const
{
    if (m_textures.find(key) == m_textures.end()) {
        Logger::GetInstance().AddLog(Level::L4, "No " + key + " in TextureController"); // TODO : Level
        return;
    }

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
        Logger::GetInstance().AddLog(Level::L4, "Try to delete a texture not in m_textures: " + key); // TODO : Level
    }
}
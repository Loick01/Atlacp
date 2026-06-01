#pragma once

#include <map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "system/type.hpp"

// Should add a FontController instead of having TextureController::m_fonts ?

struct Texture
{
    SDL_Texture* texture;
    unsigned int count;

    Texture() {
        texture = nullptr;
        count = 0;
    }
};

struct Font
{
    TTF_Font* font;
    unsigned int count;

    Font() {
        font = nullptr;
        count = 0;
    }
};

class TextureController
{
    private:
        std::map<TextureKey,Texture> m_textures;
        std::map<TextureKey,Font> m_fonts;
        SDL_Renderer* m_windowRenderer;

    public:
        TextureController(SDL_Renderer* windowRenderer);
        ~TextureController();

        TTF_Font* GetFont(const TextureKey& textureKey) const;
        
        // Try to merge LoadTextureFromFile and LoadTextureFromText
        void LoadTextureFromFile(const std::string& textureFilepath, const TextureKey& textureKey, int &textureWidth, int& textureHeight);
        void LoadTextureFromText(const TextureKey& fontKey, const TextureKey& textureKey, const std::string& text, 
            int &textureWidth, int& textureHeight, const SDL_Color textColor, const int maxWidth);

        void LoadFontFromFile(const std::string& fontFilepath, const TextureKey& textureKey, const int fontSize);
        void RenderTexture(const TextureKey& textureKey, const SDL_Rect& src, const SDL_Rect& dst) const;
        void DeleteTexture(const TextureKey& textureKey);
};
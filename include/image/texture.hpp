#pragma once

#include <map>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h> // TTF_RenderUTF8_Blended_Wrapped

#include "image/font.hpp" // FontSize
#include "image/image_types.hpp" // TextureKey

struct Texture
{
    SDL_Texture* texture;
    unsigned int count;

    Texture() {
        texture = nullptr;
        count = 0;
    }
};

class TextureController
{
    private:
        std::map<TextureKey,Texture> m_textures;
        FontController& m_fontController;
        SDL_Renderer* m_windowRenderer;

    public:
        TextureController(FontController& FontController, SDL_Renderer* windowRenderer);
        ~TextureController();
        
        void CreateTextureFromSurface(SDL_Surface* surface, const TextureKey& key, const std::string& s, int& textureWidth, int& textureHeight);
        void AddTexture(const TextureKey& key, int& textureWidth, int& textureHeight); // Rename ?

        void LoadTextureFromFile(const std::string& textureFilepath, const TextureKey& key, int &textureWidth, int& textureHeight);
        void LoadTextureFromText(const FontSize fontSize, const TextureKey& key, const std::string& text, 
            int &textureWidth, int& textureHeight, const SDL_Color textColor, const int maxWidth);

        void RenderTexture(const TextureKey& key, const SDL_Rect& src, const SDL_Rect& dst) const;
        void DeleteTexture(const TextureKey& key);
};
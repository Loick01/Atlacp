#pragma once

#include <iostream>
#include <map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "type.hpp"

// Should add a FontController instead of having TextureController::m_fonts ?

class TextureController
{
    private:
        std::map<TextureKey,SDL_Texture*> m_textures;
        std::map<TextureKey,TTF_Font*> m_fonts;
        SDL_Renderer* m_window_renderer;

    public:
        TextureController(SDL_Renderer* window_renderer);
        ~TextureController();

        TTF_Font* GetFont(const TextureKey& texture_key) const;
        void LoadTextureFromText(const TextureKey& font_key, const TextureKey& texture_key, const std::string& text, 
            int &texture_width, int& texture_height, const SDL_Color text_color, const int max_width);
        void LoadImageFromFile(const std::string& texture_filepath, const TextureKey& texture_key, int &texture_width, int& texture_height);
        void LoadFontFromFile(const std::string& font_filepath, const TextureKey& texture_key, const int font_size);
        void RenderTexture(const TextureKey& texture_key, const SDL_Rect& src, const SDL_Rect& dst) const;
        void DeleteTexture(const TextureKey& texture_key);
};
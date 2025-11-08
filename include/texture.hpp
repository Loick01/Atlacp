#pragma once

#include <iostream>
#include <map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "type.hpp"

class TextureController
{
    private:
        std::map<TextureKey,SDL_Texture*> m_textures;
        SDL_Renderer* m_window_renderer;

    public:
        TextureController(SDL_Renderer* window_renderer);
        ~TextureController();

        void LoadTextureFromFile(const std::string& texture_filepath, const TextureKey& texture_key,
            unsigned int &texture_width, unsigned int& texture_height);
        void RenderTexture(const TextureKey& texture_key, const SDL_Rect& src, const SDL_Rect& dst) const;
        void DeleteTexture(const TextureKey& texture_key);
};
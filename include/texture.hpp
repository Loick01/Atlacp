#pragma once

#include <iostream>
#include <map>
#include <string>

#include <SDL2/SDL.h>

class TextureController
{
    private:
        // Should use unordered_map ?
        static std::map<std::string,SDL_Texture*> m_textures;
        SDL_Renderer* m_window_renderer;

    public:
        TextureController(SDL_Renderer* window_renderer);
        ~TextureController();

        void LoadTextureFromFile(const char* filepath);
        void DeleteTexture(const char* texture_name);
        void RenderTexture(const char* texture_name, const SDL_Rect position);
};
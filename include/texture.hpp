#pragma once

#include <iostream>
#include <map>
#include <string>

#include <SDL2/SDL.h>

class TextureController
{
    private:
        // Should use unordered_map ?
        static std::map<std::string,SDL_Surface*> m_textures;

    public:
        TextureController();
        ~TextureController();

        void LoadTextureFromFile(const char* filepath);
        void DeleteTexture(const char* texture_name);
        void DrawTexture(const char* texture_name, SDL_Surface* window_surface, SDL_Rect position);
};
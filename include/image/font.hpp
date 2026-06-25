#pragma once

#include <map>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "image/image_types.hpp" // FontKey

class FontController
{
    private:
        std::map<FontSize,TTF_Font*> m_fonts; // One font only, with different size
        unsigned int m_smallTextSize;
        
        void LoadFontForSize(const std::string& fontFilepath, const FontSize fontSize);
        
    public:
        FontController(const std::string& fontFilepath);  // fontFilepath is not the full path, just the filename in the font directory
        ~FontController();

        TTF_Font* GetFontForSize(const FontSize fontSize) const;
};


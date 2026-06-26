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
        
        TTF_Font* GetFontForSize(const FontSize fontSize) const;
        void LoadFontForSize(const std::string& fontFilepath, const FontSize fontSize, const unsigned int textSize);
        
    public:
        FontController(const std::string& fontFilepath);  // fontFilepath is not the full path, just the filename in the font directory
        ~FontController();

        SDL_Surface* GenerateSurfaceFromText(const FontSize fontSize, const std::string& text, const SDL_Color textColor, const unsigned int maxWidth) const;
        SDL_Surface* GenerateSurfaceFromTextUnwrapped(const FontSize fontSize, const std::string& text, const SDL_Color textColor) const;
};


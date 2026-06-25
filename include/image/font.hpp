#pragma once

#include <map>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "image/image_types.hpp" // FontKey

enum class FontSize // Should be image_types.hpp ?
{
    Small // Will have Medium, High, etc.
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

class FontController
{
    private:
        std::map<FontKey,Font> m_fonts;
        unsigned int m_smallTextSize;
        
    public:
        FontController(const std::string& fontFilepath);  // fontFilepath is not the full path, just the filename in the font directory
        ~FontController();

        TTF_Font* GetFontForSize(const FontSize fontSize) const;

        void LoadFontFromFile(const std::string& fontFilepath, const FontKey& key, const unsigned int fontSize);

        void DeleteFont(const FontKey& key);
};


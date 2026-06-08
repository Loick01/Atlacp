#pragma once

#include <map>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "image/image_types.hpp" // FontKey

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

    public:
        FontController();
        ~FontController();

        TTF_Font* GetFont(const FontKey& key) const;

        void LoadFontFromFile(const std::string& fontFilepath, const FontKey& key, const int fontSize);

        void DeleteFont(const FontKey& key);
};


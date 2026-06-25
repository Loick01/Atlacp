#include "image/font.hpp"

#include <stdexcept>

FontController::FontController(const std::string& fontFilepath)
{
    if (TTF_Init() < 0) 
        throw std::runtime_error("Failed to initialize SDL font library\n" + std::string(TTF_GetError()));
    
    // TTF_Font are now created and deleted from the FontController
    
    unsigned int minBound = 1;
    unsigned int maxBound = 100; // How to get a max value ?
    const std::string smallSizeExampleText = "Hello world Hello world Hello world He";
    // while () {
        const unsigned int averageSize = (minBound+maxBound)/2;
        // std::cout << "Trying " << averageSize << "\n";
        LoadFontForSize("../assets/ui/fonts/"+fontFilepath+".ttf", FontSize::Small, averageSize);
        SDL_Surface* surface = GenerateSurfaceFromText(FontSize::Small, smallSizeExampleText, SDL_Color{0,0,0,255}, 1600); // Not 1600, will be window width I think ?
        
        std::cout << surface->w << "\n";
    
        SDL_FreeSurface(surface); // Free the temp SDL_Surface
    // }

    m_smallTextSize = 24;
    LoadFontForSize("../assets/ui/fonts/"+fontFilepath+".ttf", FontSize::Small, m_smallTextSize);
}

FontController::~FontController()
{
    for (const std::pair<const FontSize, TTF_Font*>& p : m_fonts)
        TTF_CloseFont(p.second);
    TTF_Quit();
}

TTF_Font* FontController::GetFontForSize(const FontSize fontSize) const
{
    return m_fonts.at(fontSize);
}

void FontController::LoadFontForSize(const std::string& fontFilepath, const FontSize fontSize, const unsigned int textSize)
{
    if (m_fonts.find(fontSize) != m_fonts.end())
        TTF_CloseFont(m_fonts[fontSize]);

    TTF_Font* font = TTF_OpenFont(fontFilepath.c_str(), textSize);
    if (!font) 
        throw std::runtime_error("Failed to load this font : " + fontFilepath + "\n" + std::string(TTF_GetError()));
    m_fonts[fontSize] = font;
}

SDL_Surface* FontController::GenerateSurfaceFromText(const FontSize fontSize, const std::string& text, const SDL_Color textColor, const unsigned int maxWidth) const
{
    SDL_Surface* surface = TTF_RenderUTF8_Blended_Wrapped(GetFontForSize(fontSize), text.c_str(), textColor, maxWidth); 
    if (!surface) 
            throw std::runtime_error("Failed to create a surface for this text : " + text + "\n" + std::string(SDL_GetError()));
    return surface; // This surface is free when calling TextureController::CreateTextureFromSurface()
}
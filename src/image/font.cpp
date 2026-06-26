#include "image/font.hpp"

#include <stdexcept>

FontController::FontController(const std::string& fontFilepath)
{
    if (TTF_Init() < 0) 
        throw std::runtime_error("Failed to initialize SDL font library\n" + std::string(TTF_GetError()));
    
    // TTF_Font are now created and deleted from the FontController
    
    const unsigned int maxAllowedWidth = 750; // Remove
    const unsigned int minAllowedWidth = 735; // Remove
    unsigned int minBound = 1;
    unsigned int maxBound = 100; // How to get a max value ?
    const std::string smallSizeExampleText = "Hello world Hello world Hello world Hell";
    while (maxBound - minBound > 1) {
        const unsigned int averageSize = (minBound+maxBound)/2;
        LoadFontForSize("../assets/ui/fonts/"+fontFilepath+".ttf", FontSize::Small, averageSize);
        SDL_Surface* surface = GenerateSurfaceFromTextUnwrapped(FontSize::Small, smallSizeExampleText, SDL_Color{0,0,0,255});
        
        if (surface->w > maxAllowedWidth)
            maxBound = averageSize;
        else if (surface->w < minAllowedWidth)
            minBound = averageSize;
        else {
            m_smallTextSize = averageSize;
            break;
        }
        SDL_FreeSurface(surface); // Free the temp SDL_Surface
    }

    // Don't need to call LoadFontForSize, it has been already done in the while loop
    std::cout << "Final size for FontSize::Small = " << m_smallTextSize << "\n";
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
    SDL_Surface* surface = TTF_RenderUTF8_Blended_Wrapped(GetFontForSize(fontSize), text.c_str(), textColor, maxWidth); // TTF_RenderUTF8_Solid_Wrapped
    if (!surface) 
            throw std::runtime_error("Failed to create a surface for this text : " + text + "\n" + std::string(SDL_GetError()));
    return surface; // This surface is free when calling TextureController::CreateTextureFromSurface()
}

SDL_Surface* FontController::GenerateSurfaceFromTextUnwrapped(const FontSize fontSize, const std::string& text, const SDL_Color textColor) const
{
    SDL_Surface* surface = TTF_RenderUTF8_Blended(GetFontForSize(fontSize), text.c_str(), textColor); // TTF_RenderUTF8_Solid
    if (!surface) 
            throw std::runtime_error("Failed to create a surface for this text : " + text + "\n" + std::string(SDL_GetError()));
    return surface; // This surface is free when calling TextureController::CreateTextureFromSurface()
}
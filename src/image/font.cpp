#include "image/font.hpp"

#include <stdexcept>

FontController::FontController(const std::string& fontFilepath)
{
    if (TTF_Init() < 0) 
        throw std::runtime_error("Failed to initialize SDL font library\n" + std::string(TTF_GetError()));
    
    // TTF_Font are now created and deleted from the FontController
    
    // unsigned int minBound = 1;
    // unsigned int maxBound = 100; // How to get a max value ?
    // const std::string smallSizeExampleText = "Hello world Hello world Hello world He";
    // while () {
    //     const unsigned int averageSize = (minBound+maxBound)/2;
    //     std::cout << "Trying " << averageSize << "\n";
    //     // Generate the image from smallSizeExampleText with averageSize as font size
    // }

    m_smallTextSize = 24;
    LoadFontForSize("../assets/ui/fonts/"+fontFilepath+".ttf", FontSize::Small);
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

void FontController::LoadFontForSize(const std::string& fontFilepath, const FontSize fontSize)
{
    TTF_Font* font = TTF_OpenFont(fontFilepath.c_str(), m_smallTextSize); // Not m_smallTextSize
    if (!font) 
        throw std::runtime_error("Failed to load this font : " + fontFilepath + "\n" + std::string(TTF_GetError()));
    m_fonts[fontSize] = font;
}
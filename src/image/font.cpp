#include "image/font.hpp"

#include <stdexcept>

FontController::FontController(const std::string& fontFilepath)
{
    if (TTF_Init() < 0) 
        throw std::runtime_error("Failed to initialize SDL font library\n" + std::string(TTF_GetError()));
    
    // TTF_Font are now created and deleted from the FontController
    
    m_smallTextSize = 24;
    // unsigned int minBound = 1;
    // unsigned int maxBound = 100; // How to get a max value ?
    // const std::string smallSizeExampleText = "Hello world Hello world Hello world He";
    // while () {
    //     const unsigned int averageSize = (minBound+maxBound)/2;
    //     std::cout << "Trying " << averageSize << "\n";
    //     // Generate the image from smallSizeExampleText with averageSize as font size
    // }

    const std::string smallSizeFontKey = fontFilepath + "_" + std::to_string(m_smallTextSize);
    LoadFontFromFile("../assets/ui/fonts/"+fontFilepath+".ttf", smallSizeFontKey, m_smallTextSize);
}

FontController::~FontController()
{
    for (const std::pair<const FontKey, Font>& p : m_fonts)
        TTF_CloseFont(p.second.font);
    TTF_Quit();
}

TTF_Font* FontController::GetFontForSize(const FontSize fontSize) const
{
    switch (fontSize) {
        case FontSize::Small :
            return m_fonts.begin()->second.font; // TODO
        default :   
            throw std::runtime_error("Unknown FontSize value");
    } 
    // return m_fonts.at(key).font; // No verifications for the moment
}

void FontController::LoadFontFromFile(const std::string& fontFilepath, const FontKey& key, const unsigned int fontSize)
{
    if (m_fonts.find(key) == m_fonts.end()){
        TTF_Font* font = TTF_OpenFont(fontFilepath.c_str(), fontSize);
        if (!font) 
            throw std::runtime_error("Failed to load this font : " + fontFilepath + "\n" + std::string(TTF_GetError()));

        m_fonts[key].count = 1;
        m_fonts[key].font = font;
    } else {
        m_fonts[key].count++;
    }
}

void FontController::DeleteFont(const FontKey& key)
{
    std::map<FontKey, Font>::iterator it = m_fonts.find(key);
    if (it != m_fonts.end()){
        if (it->second.count == 1) {
            TTF_CloseFont(it->second.font);
            m_fonts.erase(it);
        } else {
            it->second.count--;
        }
    } else {
        throw std::runtime_error("Try to delete a font not in FontController : " + key);
    }
}
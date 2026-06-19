#include "image/font.hpp"

#include <stdexcept>

FontController::FontController()
{
    if (TTF_Init() < 0) 
        throw std::runtime_error("Failed to initialize SDL font library\n" + std::string(TTF_GetError()));
}

FontController::~FontController()
{
    for (const std::pair<const FontKey, Font>& p : m_fonts){
        std::cout << "Should not be here --> Deleting " << p.first << " in ~FontController\n"; // Will be removed
        TTF_CloseFont(p.second.font);
    }
    TTF_Quit();
}

TTF_Font* FontController::GetFont(const FontKey& key) const
{
    return m_fonts.at(key).font; // No verifications for the moment
}

void FontController::LoadFontFromFile(const std::string& fontFilepath, const FontKey& key, const int fontSize)
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
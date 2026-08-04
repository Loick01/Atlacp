#include "ui/ui_animated_element.hpp"

#include <SDL2/SDL_rect.h> 

#include "core/path.hpp"
#include "image/texture.hpp" // Try to remove ? (TextureController::RenderTexture should not be call directly in UiAnimatedElement::DrawTexture() ?)

UiAnimatedElement::UiAnimatedElement(const FileReader& fileReader, TextureController& textureController, const UiKey& key, const std::string& animationPath):
    UiElement(textureController, key, AssetDirectory::Spritesheet+animationPath), m_animation(fileReader, animationPath)
{}

Animation& UiAnimatedElement::GetAnimation()
{
    return m_animation;
}

void UiAnimatedElement::UpdateSpriteSizeFromAnimation()
{
    const AreaSize spriteSize = m_animation.GetSpriteSize();
    m_textureWidth = spriteSize.x;
    m_textureHeight = spriteSize.y;
}

void UiAnimatedElement::DrawTexture() const
{
    // Do not add children UiAnimatedElement (?)
    // if (m_shouldDraw) ? (from ScreenDrawable::DrawTexture)
    const Vec2 sprite = m_animation.GetCurrentSprite();
    const SDL_Rect src{sprite.x, sprite.y, m_textureWidth, m_textureHeight};
    const SDL_Rect dst{m_position.x, m_position.y, static_cast<int>(m_textureWidth*m_zoom), static_cast<int>(m_textureHeight*m_zoom)};
    m_textureController.RenderTexture(m_textureKey, src, dst);
}
#include "ui/element/ui_animated_element.hpp"

#include <SDL2/SDL_rect.h> 

#include "core/path.hpp"
#include "image/texture.hpp" // TODO : Try to remove ? (TextureController::RenderTexture should not be call directly in UiAnimatedElement::DrawTexture() ?)

UiAnimatedElement::UiAnimatedElement(const FileReader& fileReader, TextureController& textureController, const UiKey& key, const std::string& animationPath):
    UiElement(textureController, key, std::string(AssetDirectory::Spritesheet)+animationPath), m_animation(fileReader, animationPath)
{}

SpriteAnimation& UiAnimatedElement::GetAnimation()
{
    return m_animation;
}

void UiAnimatedElement::UpdateSpriteSizeFromAnimation()
{
    m_textureSize = m_animation.GetSpriteSize();
}

void UiAnimatedElement::DrawTexture() const
{
    // TODO : Do not add children UiAnimatedElement ?
    // TODO : if (m_shouldDraw) ? (from ScreenDrawable::DrawTexture)
    const Vec2 sprite = m_animation.GetCurrentSprite();
    const SDL_Rect src{sprite.x, sprite.y, m_textureSize.x, m_textureSize.y};
    const SDL_Rect dst{m_position.x, m_position.y, GetTextureSize().x, GetTextureSize().y};
    m_textureController.RenderTexture(m_textureKey, src, dst);
}
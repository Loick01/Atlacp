#pragma once

#include "animation/sprite_animation.hpp"
#include "ui/element/ui_element.hpp"

class UiAnimatedElement : public UiElement // Rename UiAnimationElement ?
{
    private:    
        SpriteAnimation m_animation;

    public:
        UiAnimatedElement(const FileReader& fileReader, TextureController& textureController, const UiKey& key, const std::string& animationPath);

        SpriteAnimation& GetAnimation();
        void UpdateSpriteSizeFromAnimation();
        void DrawTexture() const override;
};
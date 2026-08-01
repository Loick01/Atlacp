#pragma once

#include "animation/animation.hpp"
#include "ui/ui_element.hpp"

class UiAnimatedElement : public UiElement // Rename UiAnimationElement ?
{
    private:    
        Animation m_animation;

    public:
        UiAnimatedElement(const FileReader& fileReader, TextureController& textureController, const UiKey& key, const std::string& animationPath);

        Animation& GetAnimation();
        void UpdateSpriteSizeFromAnimation();
        void DrawTexture() const override;
};
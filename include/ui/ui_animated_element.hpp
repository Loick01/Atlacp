#pragma once

#include "animation/animation.hpp"
#include "ui/ui_element.hpp"

class UiAnimatedElement : public UiElement
{
    private:    
        Animation m_animation;

    public:
        UiAnimatedElement(const FileReader& fileReader, TextureController& textureController, const UiKey& key, const std::string& animationPath);

        Animation& GetAnimation();
        void GetSpriteSizeFromAnimation();
        void DrawTexture() const override;
};
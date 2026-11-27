#pragma once

#include "animation/sprite_animation.hpp"
#include "ui/element/ui_element.hpp"

struct Time; 

class UiAnimatedElement : public UiElement // TODO : Rename UiAnimationElement ?
{
    private:    
        SpriteAnimation m_animation;
        const Time& m_time;

    public:
        UiAnimatedElement(const FileReader& fileReader, TextureController& textureController, const Time& time, const UiKey& key, const std::string& animationPath);

        SpriteAnimation& GetAnimation();
        void UpdateSpriteSizeFromAnimation();
        void DrawTexture() override;
};
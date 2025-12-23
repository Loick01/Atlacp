#pragma once

#include "drawable.hpp"

class UiElement : public ScreenDrawable
{
    private:

    public:
        UiElement(TextureController& texture_controller, const std::string& texture_filepath, const ScreenPosition position);
        void DrawTexture() const override;
};
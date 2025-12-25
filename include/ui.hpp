#pragma once

#include "drawable.hpp"

class UiElement : public ScreenDrawable
{
    private:

    public:
        UiElement(TextureController& texture_controller, const std::string& texture_filepath);
};

class UiController
{
    protected:
        std::vector<UiElement*> m_ui_elements; // Need dynamic dispatch

    public:
        void Draw() const;
};

class GameplayUiController : public UiController
{
    private:
        UiElement m_dialog_box; // Will be in m_ui_elements

    public:
        GameplayUiController(TextureController& texture_controller, const Camera& camera);
};
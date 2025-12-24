#include "ui.hpp"

UiElement::UiElement(TextureController& texture_controller, const std::string& texture_filepath, const ScreenPosition position):
    ScreenDrawable(texture_controller, texture_filepath, ScenePosition{0, 0})
{

}

void UiElement::DrawTexture() const
{
    const SDL_Rect src{0, 0, m_texture_width, m_texture_height};
    const float zoom = 1.0f;
    const SDL_Rect dst{m_position.x, m_position.y,
                       static_cast<int>(m_texture_width*zoom), static_cast<int>(m_texture_height*zoom)};
    m_texture_controller.RenderTexture(m_texture_key, src, dst);
}

void UiController::Draw() const
{
    for (UiElement* e : m_ui_elements)
        e->DrawTexture();
}

GameplayUiController::GameplayUiController(TextureController& texture_controller):
    m_dialog_box(texture_controller, "../assets/ui/box.png", ScreenPosition{0, 0})
{
    m_ui_elements.push_back(&m_dialog_box);
}
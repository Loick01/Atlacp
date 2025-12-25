#include "ui.hpp"

UiElement::UiElement(TextureController& texture_controller, const std::string& texture_filepath):
    ScreenDrawable(texture_controller, texture_filepath)
{

}

void UiController::Draw() const
{
    for (UiElement* e : m_ui_elements)
        e->DrawTexture();
}

GameplayUiController::GameplayUiController(TextureController& texture_controller, const Camera& camera):
    m_dialog_box(texture_controller, "../assets/ui/box.png")
{
    // Will be done to find zoom and position for each UiElement
    const ScenePosition viewport_size = camera.GetViewport();
    const float scale = 0.5; // Wanted scale value in [0, 1]
    m_dialog_box.SetZoom((viewport_size.x*scale)/m_dialog_box.GetTextureWidth());
    // Could be improve : GetTextureWidth/Height could use camera zoom for SceneDrawable and zoom for ScreenDrawable (few updates will be necessary in EventController)
    const ScreenPosition padding = {50, 50};
    const ScreenPosition new_size = m_dialog_box.GetSize()*m_dialog_box.GetZoom(); 
    const ScreenPosition delta_size = viewport_size - new_size;
    m_dialog_box.SetScreenPosition(camera.GetScreenOffset() + ScreenPosition{delta_size.x/2, delta_size.y - padding.y});
    m_ui_elements.push_back(&m_dialog_box);
}
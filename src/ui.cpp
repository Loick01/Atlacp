#include "ui.hpp"

UiElement::UiElement(TextureController& texture_controller, const std::string& texture_filepath):
    ScreenDrawable(texture_controller, texture_filepath)
{

}

UiElement::UiElement(TextureController& texture_controller):
    ScreenDrawable(texture_controller)
{

}

void UiElement::AddChild(UiElement* child)
{
    m_childs.push_back(child);
}

void UiElement::DrawTexture() const
{
    ScreenDrawable::DrawTexture();
    for (const UiElement* e : m_childs)
        e->DrawTexture();
}

void UiElement::UpdatePosition(const ScreenPosition sp)
{
    SetScreenPosition(sp);
    for (UiElement* e : m_childs)
        e->UpdatePosition(GetScreenPosition()); // Make sure to update childs position after updating this->position
}

TextArea::TextArea(TextureController& texture_controller, const std::string& font_filepath, const SDL_Color color):
    UiElement(texture_controller), m_text_color(color)
{
    const unsigned int text_size = 48;
    m_font_key = font_filepath; // font_filepath is not the full path, just the filename in the font directory
    texture_controller.LoadFontFromFile("../assets/ui/fonts/"+font_filepath, m_font_key, text_size);
}

void TextArea::SetText(const std::string& text)
{
    m_texture_key = text; // Will use something else than just text as texture key
    m_texture_controller.LoadTextureFromText(m_font_key, m_texture_key, text, m_texture_width, m_texture_height, m_text_color);
}

void UiController::Draw() const
{
    m_root->DrawTexture();
}

GameplayUiController::GameplayUiController(TextureController& texture_controller, const Camera& camera, const std::string& font_filepath):
    m_dialog_box(texture_controller, "../assets/ui/box.png"), m_text_area(texture_controller, font_filepath)
{
    m_root = &m_dialog_box;
    m_dialog_box.AddChild(&m_text_area);

    m_text_area.SetText("Hello world !");

    // Will be done to find zoom and position for each UiElement
    const ScenePosition viewport_size = camera.GetViewport();
    const float scale = 0.5; // Wanted scale value in [0, 1]
    m_dialog_box.SetZoom((viewport_size.x*scale)/m_dialog_box.GetTextureWidth());
    // Could be improve : GetTextureWidth/Height could use camera zoom for SceneDrawable and zoom for ScreenDrawable (few updates will be necessary in EventController)
    const ScreenPosition padding = {50, 50};
    const ScreenPosition new_size = m_dialog_box.GetSize()*m_dialog_box.GetZoom(); 
    const ScreenPosition delta_size = viewport_size - new_size;

    m_dialog_box.UpdatePosition(camera.GetScreenOffset() + ScreenPosition{delta_size.x/2, delta_size.y - padding.y});
}
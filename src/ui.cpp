#include "ui.hpp"

UiElement::UiElement(TextureController& texture_controller, const std::string& texture_filepath, const ScreenPosition local_position):
    ScreenDrawable(texture_controller, texture_filepath), m_local_position(local_position)
{
    
}

UiElement::UiElement(TextureController& texture_controller, const ScreenPosition local_position):
    ScreenDrawable(texture_controller), m_local_position(local_position)
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

void UiElement::UpdatePosition(const ScreenPosition parent_position)
{
    SetScreenPosition(parent_position + m_local_position); // ScreenDrawable::m_position is the global position
    for (UiElement* e : m_childs)
        e->UpdatePosition(GetScreenPosition()); // Make sure to update childs position after updating m_position
}

void UiElement::SetLocalPosition(const ScreenPosition local_position)
{
    m_local_position = local_position;
}

TextArea::TextArea(TextureController& texture_controller, const std::string& font_filepath, const SDL_Color color):
    UiElement(texture_controller), m_text_color(color)
{
    const unsigned int text_size = 24;
    m_font_key = font_filepath; // font_filepath is not the full path, just the filename in the font directory
    texture_controller.LoadFontFromFile("../assets/ui/fonts/"+font_filepath, m_font_key, text_size);
}

void TextArea::SetText(const std::string& text, const int max_width)
{
    m_texture_key = text; // Will use something else than just text as texture key
    m_texture_controller.LoadTextureFromText(m_font_key, m_texture_key, text, m_texture_width, m_texture_height, m_text_color, max_width);
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
    
    // Will be done to find zoom and position for each UiElement
    const ScenePosition viewport_size = camera.GetViewport();
    const float scale = 0.5; // Wanted scale value in [0, 1]
    m_dialog_box.SetZoom((viewport_size.x*scale)/m_dialog_box.GetTextureWidth());
    // Could be improve : GetTextureWidth/Height could use camera zoom for SceneDrawable and zoom for ScreenDrawable (few updates will be necessary in EventController)
    const ScreenPosition margin = {50, 50};
    const ScreenPosition new_size = m_dialog_box.GetSize()*m_dialog_box.GetZoom(); 
    const ScreenPosition delta_size = viewport_size - new_size;
    const ScreenPosition box_position = camera.GetScreenOffset() + ScreenPosition{delta_size.x/2, delta_size.y - margin.y};
    m_dialog_box.SetLocalPosition(box_position);

    // Should use Pair<int>
    const int padding_x = new_size.x*0.04;
    const int padding_y = new_size.y*0.15;
    m_text_area.SetLocalPosition(ScreenPosition{padding_x, padding_y});
    m_text_area.SetText("Hello world ! This is an example of a long sentence to test how the text is wrapped by SDL_ttf...", new_size.x-2*padding_x); // Use 2*local_position.x as max text width

    m_dialog_box.UpdatePosition(); // For now, dialog box is the root of UiElement graph, with global position = local position
    // m_dialog_box.SetLocalPosition(ScreenPosition{1000,500}); // Every changes of position must update children, by calling UpdatePosition()
    // m_dialog_box.UpdatePosition();
}
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

void UiElement::ComputeZoom(const ScreenPosition parent_size, const float scale, const ScaleAxis axis)
{
    switch(axis){
        case ScaleAxis::Width:
            SetZoom((parent_size.x*scale)/GetTextureWidth());
            break;
        case ScaleAxis::Height:
            SetZoom((parent_size.y*scale)/GetTextureHeight());
            break;
        default:
            std::cout << "Unknown ScaleAxis value\n"; // Error
            break;
    }
}

void UiElement::ComputePosition(const ScreenPosition parent_size, const Anchor x_anchor, const Anchor y_anchor)
{
    ScreenPosition final_position = {0,0};
    // If ComputePosition is called on a TextArea, the zoom here must be 1.0f (text size is handled by the font), I should use a virtual function to get the zoom here
    const ScreenPosition new_size = GetSize()*GetZoom(); // Need drawing size (including the zoom) to get the position (so ComputePosition must be called after ComputeZoom) 
    switch(x_anchor){
        case Anchor::Left:
            final_position.x = 0;
            break;
        case Anchor::Center:
            final_position.x = (parent_size.x-new_size.x)/2;
            break;
        case Anchor::Right:
            final_position.x = parent_size.x-new_size.x;
            break;
        default:
            std::cout << "Incorrect Anchor value for x axis\n"; // Error
    }
    switch(y_anchor){
        case Anchor::Top:
            final_position.y = 0;
            break;
        case Anchor::Center:
            final_position.y = (parent_size.y-new_size.y)/2;
            break;
        case Anchor::Bottom:
            final_position.y = parent_size.y-new_size.y;
            break;
        default:
            std::cout << "Incorrect Anchor value for y axis\n"; // Error
    }
    SetLocalPosition(final_position);
}

GameplayUiController::GameplayUiController(TextureController& texture_controller, const Camera& camera, const std::string& font_filepath):
    m_dialog_box(texture_controller, "../assets/ui/box.png"), m_faceset(texture_controller, "../assets/ui/faceset.png"), 
    m_text_area(texture_controller, font_filepath)
{
    // For now, dialog box is the root of UiElement graph, with global position = local position
    // Technically, m_root sould be the camera viewport, but it's not a UiElement
    m_root = &m_dialog_box;
    m_dialog_box.AddChild(&m_faceset);
    m_dialog_box.AddChild(&m_text_area);

    const ScreenPosition viewport_size = camera.GetViewport();
    m_dialog_box.ComputeZoom(viewport_size, 0.5f, ScaleAxis::Width);
    m_dialog_box.ComputePosition(viewport_size, Anchor::Center, Anchor::Bottom);
    
    const ScreenPosition dialog_box_size = m_dialog_box.GetSize()*m_dialog_box.GetZoom(); // Drawing size including the zoom
    m_faceset.ComputeZoom(dialog_box_size, 0.8f, ScaleAxis::Height);
    m_faceset.ComputePosition(dialog_box_size, Anchor::Left, Anchor::Center);

    // Could be improve : GetTextureWidth/Height could use camera zoom for SceneDrawable and zoom for ScreenDrawable (few updates will be necessary in EventController)
    //const ScreenPosition padding = {50, 50};
    //const ScreenPosition box_position = camera.GetScreenOffset() + ScreenPosition{delta_size.x/2, delta_size.y - padding.y};
    //m_dialog_box.SetLocalPosition(box_position);

    // Do not use ComputeZoom for TextArea. Text size is controlled by the font
    // Be sure to call TextArea::ComputePosition after generating the texture with SetText
    // Use parent_size.x-2*local_position.x as max text width
    const int padding_x = dialog_box_size.x*0.04;
    const int padding_y = dialog_box_size.y*0.15;
    m_text_area.SetText("Hello world ! This is an example of a long sentence to test how the text is wrapped by SDL_ttf...", dialog_box_size.x-2*padding_x);
    m_text_area.ComputePosition(dialog_box_size, Anchor::Left, Anchor::Center);

    // Should use Pair<int>
    // const int padding_x = dialog_box_size.x*0.04;
    // const int padding_y = dialog_box_size.y*0.15;
    // m_text_area.SetLocalPosition(ScreenPosition{padding_x, padding_y});

    m_dialog_box.UpdatePosition(); // Call UpdatePosition on the root UiElement
}
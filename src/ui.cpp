#include "ui.hpp"

UiElement::UiElement(TextureController& texture_controller, const std::string& texture_filepath, const ScreenPosition local_position):
    ScreenDrawable(texture_controller, texture_filepath), m_local_position(local_position)
{
    
}

UiElement::UiElement(TextureController& texture_controller, const ScreenPosition local_position):
    ScreenDrawable(texture_controller), m_local_position(local_position)
{
    
}

void UiElement::SetParentSize(const AreaSize parent_size)
{
    m_parent_size = parent_size;
}

void UiElement::AddChild(UiElement* child)
{
    child->SetParentSize(this->GetSize());
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

void UiElement::AddLocalPosition(const ScreenPosition padding)
{
    m_local_position += padding;
}

TextArea::TextArea(TextureController& texture_controller, const std::string& font_filepath, const SDL_Color color):
    UiElement(texture_controller), m_text_color(color)
{
    const unsigned int font_size = 24;
    m_font_key = font_filepath + std::to_string(font_size); // font_filepath is not the full path, just the filename in the font directory
    texture_controller.LoadFontFromFile("../assets/ui/fonts/"+font_filepath+".ttf", m_font_key, font_size);
}

void TextArea::SetText(const std::string& text)
{
    m_texture_key = text; // Will use something else than just text as texture key
    m_texture_controller.LoadTextureFromText(m_font_key, m_texture_key, text, m_texture_width, m_texture_height, m_text_color, m_max_width);
}

void TextArea::SetMaxWidth(const float scale)
{
    m_max_width = m_parent_size.x*scale;
}

void UiController::Draw() const
{
    m_root->DrawTexture();
}

void UiElement::ComputeZoom(const float scale, const Axis axis)
{
    switch(axis){
        case Axis::Width:
            SetZoom((m_parent_size.x*scale)/GetTextureWidth());
            break;
        case Axis::Height:
            SetZoom((m_parent_size.y*scale)/GetTextureHeight());
            break;
        default:
            std::cout << "Unknown Axis value\n"; // Error
            break;
    }
}

void UiElement::ComputePosition(const Anchor x_anchor, const Anchor y_anchor)
{
    ScreenPosition final_position = {0,0};
    // If ComputePosition is called on a TextArea, the zoom here must be 1.0f (text size is handled by the font), I should use a virtual function to get the zoom here
    const AreaSize new_size = GetSize(); // Need drawing size (including the zoom) to get the position (so ComputePosition must be called after ComputeZoom) 
    switch(x_anchor){
        case Anchor::Left:
            final_position.x = 0;
            break;
        case Anchor::Center:
            final_position.x = (m_parent_size.x-new_size.x)/2;
            break;
        case Anchor::Right:
            final_position.x = m_parent_size.x-new_size.x;
            break;
        default:
            std::cout << "Incorrect Anchor value for x axis\n"; // Error
    }
    switch(y_anchor){
        case Anchor::Top:
            final_position.y = 0;
            break;
        case Anchor::Center:
            final_position.y = (m_parent_size.y-new_size.y)/2;
            break;
        case Anchor::Bottom:
            final_position.y = m_parent_size.y-new_size.y;
            break;
        default:
            std::cout << "Incorrect Anchor value for y axis\n"; // Error
    }
    SetLocalPosition(final_position);
}

void UiElement::AddPadding(const Axis source_axis, const Axis padding_axis, const float scale)
{
    ScreenPosition padding = {0,0};
    float v = 0.f;

    switch(source_axis){
        case Axis::Width:
            v = m_parent_size.x*scale;
            break;
        case Axis::Height:
            v = m_parent_size.y*scale;
            break;
    }

    switch(padding_axis){
        case Axis::Width:
            padding.x = v;
            break;
        case Axis::Height:
            padding.y = v;
            break;
        default:
            std::cout << "Unknown Axis value\n"; // Will throw error
            break;
    }

    AddLocalPosition(padding);
}

void UiElement::AddPadding(const Axis padding_axis, const float scale)
{
    AddPadding(padding_axis, padding_axis, scale);
}

void UiController::SetRoot(UiElement* ui_root, const AreaSize initial_size)
{
    m_root = ui_root;
    m_root->SetParentSize(initial_size);
}

GameplayUiController::GameplayUiController(TextureController& texture_controller, const Camera& camera, const std::string& font_filepath):
    m_dialog_box(texture_controller, "../assets/ui/box.png"), m_faceset(texture_controller, "../assets/ui/faceset.png"), 
    m_face(texture_controller, "../assets/ui/hunter_face.png"), m_text_area(texture_controller, font_filepath)
{
    // For now, dialog box is the root of UiElement graph, with global position = local position
    // Technically, m_root sould be the camera viewport, but it's not a UiElement
    const AreaSize viewport_size = camera.GetViewport();
    SetRoot(&m_dialog_box, viewport_size);
    m_dialog_box.ComputeZoom(0.5f, Axis::Width);
    m_dialog_box.ComputePosition(Anchor::Center, Anchor::Bottom);
    m_dialog_box.AddPadding(Axis::Height, -0.05f);
    m_dialog_box.AddChild(&m_faceset);
    m_dialog_box.AddChild(&m_text_area);
    
    m_faceset.ComputeZoom(0.7f, Axis::Height);
    m_faceset.ComputePosition(Anchor::Left, Anchor::Center);
    m_faceset.AddPadding(Axis::Height, Axis::Width, 0.15f);
    m_faceset.AddChild(&m_face);

    m_face.ComputeZoom(0.8f, Axis::Width);
    m_face.ComputePosition(Anchor::Center, Anchor::Center);

    // Do not use ComputeZoom for TextArea. Text size is controlled by the font
    // Be sure to call TextArea::ComputePosition after generating the texture with SetText
    m_text_area.SetMaxWidth(0.75f); // Find another way to get this value
    m_text_area.SetText("Hello world ! This is an example of a long sentence to test how the text is wrapped by SDL_ttf...");
    m_text_area.ComputePosition(Anchor::Left, Anchor::Center);
    m_text_area.AddPadding(Axis::Width, 0.18f);

    m_dialog_box.UpdatePosition(camera.GetScreenOffset()); // Call UpdatePosition on the root UiElement
}

EditorUiController::EditorUiController(TextureController& texture_controller, const Camera& camera, const std::string& font_filepath, const int selected_layer):
    m_dialog_box(texture_controller, "../assets/ui/box.png"), m_text_area(texture_controller, font_filepath), m_last_layer(selected_layer)
{
    const AreaSize viewport_size = camera.GetViewport();
    SetRoot(&m_dialog_box, viewport_size);
    m_dialog_box.ComputeZoom(0.3f, Axis::Width);
    m_dialog_box.ComputePosition(Anchor::Left, Anchor::Top);
    m_dialog_box.AddPadding(Axis::Width, 0.02f);
    m_dialog_box.AddPadding(Axis::Width, Axis::Height, 0.02f);
    m_dialog_box.AddChild(&m_text_area);
    
    m_text_area.SetMaxWidth(0.9f);
    m_text_area.SetText("Selected layer : " + std::to_string(m_last_layer));
    m_text_area.ComputePosition(Anchor::Left, Anchor::Center);
    m_text_area.AddPadding(Axis::Width, 0.1f);

    m_dialog_box.UpdatePosition(camera.GetScreenOffset());
}

void EditorUiController::UpdateState(const int selected_layer)
{
    if (selected_layer != m_last_layer){
        m_last_layer = selected_layer;
        m_text_area.SetText("Selected layer : " + std::to_string(m_last_layer));
    }
}

BattleUiController::BattleUiController(TextureController& texture_controller, const Camera& camera, const std::string& font_filepath):
    m_background(texture_controller, "../assets/battle/backgrounds/cavern.png"),
    m_player(texture_controller, "../assets/battle/werewolf.png"), m_enemy(texture_controller, "../assets/battle/bone_appetit.png"),
    m_player_box(texture_controller, "../assets/ui/box.png"), m_enemy_box(texture_controller, "../assets/ui/box.png"),
    m_player_info(texture_controller, font_filepath), m_enemy_info(texture_controller, font_filepath),
    m_main_box(texture_controller, "../assets/ui/box.png")
{
    const float size = 0.2f; // Will be removed
    
    const AreaSize viewport_size = camera.GetViewport();
    SetRoot(&m_background, viewport_size);
    m_background.ComputeZoom(1.0f, Axis::Height);
    m_background.ComputePosition(Anchor::Left, Anchor::Top);
    m_background.AddChild(&m_player);
    m_background.AddChild(&m_enemy);
    m_background.AddChild(&m_main_box);

    m_player.ComputeZoom(size, Axis::Width);
    m_player.ComputePosition(Anchor::Right, Anchor::Center);
    m_player.AddPadding(Axis::Width, -size);
    m_player.AddChild(&m_player_box);
    
    m_enemy.ComputeZoom(size, Axis::Width);
    m_enemy.ComputePosition(Anchor::Left, Anchor::Center);
    m_enemy.AddPadding(Axis::Width, size);
    m_enemy.AddChild(&m_enemy_box);

    m_player_box.ComputeZoom(0.8f, Axis::Width);
    m_player_box.ComputePosition(Anchor::Right, Anchor::Bottom);
    m_player_box.AddPadding(Axis::Width, size*2);
    m_player_box.AddChild(&m_player_info);

    m_enemy_box.ComputeZoom(0.8f, Axis::Width);
    m_enemy_box.ComputePosition(Anchor::Right, Anchor::Bottom);
    m_enemy_box.AddPadding(Axis::Width, size*2);
    m_enemy_box.AddChild(&m_enemy_info);

    m_player_info.SetMaxWidth(1.f);
    m_player_info.SetText("Howler");
    m_player_info.ComputePosition(Anchor::Left, Anchor::Center);
    m_player_info.AddPadding(Axis::Width, 0.1f);

    m_enemy_info.SetMaxWidth(1.f);
    m_enemy_info.SetText("Bone Appetit");
    m_enemy_info.ComputePosition(Anchor::Left, Anchor::Center);
    m_enemy_info.AddPadding(Axis::Width, 0.1f);

    m_main_box.ComputeZoom(0.5f, Axis::Width);
    m_main_box.ComputePosition(Anchor::Center, Anchor::Bottom);
    m_main_box.AddPadding(Axis::Height, -0.05f);

    m_background.UpdatePosition(camera.GetScreenOffset());
}
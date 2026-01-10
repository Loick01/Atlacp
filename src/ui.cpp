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

void TextArea::SetMaxWidth(const int max_width)
{
    m_max_width = max_width;
}

void UiController::Draw() const
{
    m_root->DrawTexture();
}

void UiElement::ComputeZoom(const AreaSize parent_size, const float scale, const Axis axis)
{
    switch(axis){
        case Axis::Width:
            SetZoom((parent_size.x*scale)/GetTextureWidth());
            break;
        case Axis::Height:
            SetZoom((parent_size.y*scale)/GetTextureHeight());
            break;
        default:
            std::cout << "Unknown Axis value\n"; // Error
            break;
    }
}

void UiElement::ComputePosition(const AreaSize parent_size, const Anchor x_anchor, const Anchor y_anchor)
{
    ScreenPosition final_position = {0,0};
    // If ComputePosition is called on a TextArea, the zoom here must be 1.0f (text size is handled by the font), I should use a virtual function to get the zoom here
    const AreaSize new_size = GetSize(); // Need drawing size (including the zoom) to get the position (so ComputePosition must be called after ComputeZoom) 
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

void UiElement::AddPadding(const Axis axis, const int ref_size, const float scale)
{
    ScreenPosition padding = {0,0};
    switch(axis){
        case Axis::Width:
            padding.x = ref_size*scale;
            break;
        case Axis::Height:
            padding.y = ref_size*scale;
            break;
        default:
            std::cout << "Unknown Axis value\n"; // Error
            break;
    }
    AddLocalPosition(padding);
}

GameplayUiController::GameplayUiController(TextureController& texture_controller, const Camera& camera, const std::string& font_filepath):
    m_dialog_box(texture_controller, "../assets/ui/box.png"), m_faceset(texture_controller, "../assets/ui/faceset.png"), 
    m_face(texture_controller, "../assets/ui/hunter_face.png"), m_text_area(texture_controller, font_filepath)
{
    // For now, dialog box is the root of UiElement graph, with global position = local position
    // Technically, m_root sould be the camera viewport, but it's not a UiElement
    m_root = &m_dialog_box;
    m_dialog_box.AddChild(&m_faceset);
    m_faceset.AddChild(&m_text_area);
    m_faceset.AddChild(&m_face);

    const AreaSize viewport_size = camera.GetViewport();
    m_dialog_box.ComputeZoom(viewport_size, 0.5f, Axis::Width);
    m_dialog_box.ComputePosition(viewport_size, Anchor::Center, Anchor::Bottom);
    m_dialog_box.AddPadding(Axis::Height, viewport_size.y, -0.05f);
    
    const AreaSize dialog_box_size = m_dialog_box.GetSize();
    m_faceset.ComputeZoom(dialog_box_size, 0.7f, Axis::Height);
    m_faceset.ComputePosition(dialog_box_size, Anchor::Left, Anchor::Center);
    m_faceset.AddPadding(Axis::Width, dialog_box_size.y, 0.15f);

    const AreaSize faceset_size = m_faceset.GetSize();
    m_face.ComputeZoom(faceset_size, 0.8f, Axis::Width);
    m_face.ComputePosition(faceset_size, Anchor::Center, Anchor::Center);

    // Do not use ComputeZoom for TextArea. Text size is controlled by the font
    // Be sure to call TextArea::ComputePosition after generating the texture with SetText
    m_text_area.SetMaxWidth(dialog_box_size.x - dialog_box_size.x*0.15f - 2*dialog_box_size.y*0.15f); // Find another way to get this value
    m_text_area.SetText("Hello world ! This is an example of a long sentence to test how the text is wrapped by SDL_ttf...");
    m_text_area.ComputePosition(faceset_size, Anchor::Left, Anchor::Center);
    m_text_area.AddPadding(Axis::Width, dialog_box_size.x, 0.15f);

    m_dialog_box.UpdatePosition(camera.GetScreenOffset()); // Call UpdatePosition on the root UiElement
}

EditorUiController::EditorUiController(TextureController& texture_controller, const Camera& camera, const std::string& font_filepath, const int selected_layer):
    m_dialog_box(texture_controller, "../assets/ui/box.png"), m_text_area(texture_controller, font_filepath), m_last_layer(selected_layer)
{
    m_root = &m_dialog_box;
    m_dialog_box.AddChild(&m_text_area);

    const AreaSize viewport_size = camera.GetViewport();
    m_dialog_box.ComputeZoom(viewport_size, 0.3f, Axis::Width);
    m_dialog_box.ComputePosition(viewport_size, Anchor::Left, Anchor::Top);
    m_dialog_box.AddPadding(Axis::Width, viewport_size.x, 0.02f);
    m_dialog_box.AddPadding(Axis::Height, viewport_size.x, 0.02f);
    
    const AreaSize dialog_box_size = m_dialog_box.GetSize();
    m_text_area.SetMaxWidth(dialog_box_size.x - dialog_box_size.x*0.1f);
    m_text_area.SetText("Selected layer : " + std::to_string(m_last_layer));
    m_text_area.ComputePosition(dialog_box_size, Anchor::Left, Anchor::Center);
    m_text_area.AddPadding(Axis::Width, dialog_box_size.x, 0.1f);

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
    m_player_info(texture_controller, font_filepath), m_enemy_info(texture_controller, font_filepath)
{
    const float size = 0.2f; // Will be removed
    m_root = &m_background;
    m_background.AddChild(&m_player);
    m_background.AddChild(&m_enemy);
    m_player.AddChild(&m_player_box);
    m_enemy.AddChild(&m_enemy_box);
    m_player_box.AddChild(&m_player_info);
    m_enemy_box.AddChild(&m_enemy_info);

    const AreaSize viewport_size = camera.GetViewport();
    m_background.ComputeZoom(viewport_size, 1.0f, Axis::Width);
    m_background.ComputePosition(viewport_size, Anchor::Left, Anchor::Top);

    const AreaSize background_size = m_background.GetSize();
    m_player.ComputeZoom(background_size, size, Axis::Width);
    m_player.ComputePosition(background_size, Anchor::Right, Anchor::Center);
    m_player.AddPadding(Axis::Width, background_size.x, -size);
    
    m_enemy.ComputeZoom(background_size, size, Axis::Width);
    m_enemy.ComputePosition(background_size, Anchor::Left, Anchor::Center);
    m_enemy.AddPadding(Axis::Width, background_size.x, size);

    const AreaSize player_size = m_player.GetSize();
    m_player_box.ComputeZoom(player_size, 0.8f, Axis::Width);
    m_player_box.ComputePosition(player_size, Anchor::Right, Anchor::Bottom);
    m_player_box.AddPadding(Axis::Width, player_size.x, size*2);

    m_enemy_box.ComputeZoom(player_size, 0.8f, Axis::Width);
    m_enemy_box.ComputePosition(player_size, Anchor::Right, Anchor::Bottom);
    m_enemy_box.AddPadding(Axis::Width, player_size.x, size*2);
    
    const AreaSize box_size = m_player_box.GetSize();
    m_player_info.SetMaxWidth(box_size.x);
    m_player_info.SetText("Howler");
    m_player_info.ComputePosition(box_size, Anchor::Left, Anchor::Center);
    m_player_info.AddPadding(Axis::Width, box_size.x, 0.1f);

    m_enemy_info.SetMaxWidth(box_size.x);
    m_enemy_info.SetText("Bone Appetit");
    m_enemy_info.ComputePosition(box_size, Anchor::Left, Anchor::Center);
    m_enemy_info.AddPadding(Axis::Width, box_size.x, 0.1f);

    m_background.UpdatePosition(camera.GetScreenOffset());
}
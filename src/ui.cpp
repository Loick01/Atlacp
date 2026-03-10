#include "ui.hpp"

UiElement::UiElement(TextureController& textureController, const std::string& textureFilepath, const ScreenPosition localPosition):
    ScreenDrawable(textureController, textureFilepath), m_localPosition(localPosition)
{
    
}

UiElement::UiElement(TextureController& textureController, const ScreenPosition localPosition):
    ScreenDrawable(textureController), m_localPosition(localPosition)
{
    
}

void UiElement::SetParentSize(const AreaSize parentSize)
{
    m_parentSize = parentSize;
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

void UiElement::UpdatePosition(const ScreenPosition parentPosition)
{
    SetScreenPosition(parentPosition + m_localPosition); // ScreenDrawable::m_position is the global position
    for (UiElement* e : m_childs)
        e->UpdatePosition(GetScreenPosition()); // Make sure to update childs position after updating m_position
}

void UiElement::SetLocalPosition(const ScreenPosition localPosition)
{
    m_localPosition = localPosition;
}

void UiElement::AddLocalPosition(const ScreenPosition padding)
{
    m_localPosition += padding;
}

TextArea::TextArea(TextureController& textureController, const std::string& fontFilepath, const SDL_Color color):
    UiElement(textureController), m_textColor(color)
{
    const unsigned int fontSize = 24;
    m_fontKey = fontFilepath + std::to_string(fontSize); // fontFilepath is not the full path, just the filename in the font directory
    textureController.LoadFontFromFile("../assets/ui/fonts/"+fontFilepath+".ttf", m_fontKey, fontSize);
}

void TextArea::SetText(const std::string& text)
{
    m_textureKey = text; // Will use something else than just text as texture key
    m_textureController.LoadTextureFromText(m_fontKey, m_textureKey, text, m_textureWidth, m_textureHeight, m_textColor, m_maxWidth);
}

void TextArea::SetMaxWidth(const float scale)
{
    m_maxWidth = m_parentSize.x*scale;
}

void UiController::Draw() const
{
    m_root->DrawTexture();
}

void UiElement::ComputeZoom(const float scale, const Axis axis)
{
    switch(axis){
        case Axis::Width:
            SetZoom((m_parentSize.x*scale)/GetTextureWidth());
            break;
        case Axis::Height:
            SetZoom((m_parentSize.y*scale)/GetTextureHeight());
            break;
        default:
            std::cout << "Unknown Axis value\n"; // Error
            break;
    }
}

void UiElement::ComputePosition(const Anchor xAnchor, const Anchor yAnchor)
{
    ScreenPosition final_position = {0,0};
    // If ComputePosition is called on a TextArea, the zoom here must be 1.0f (text size is handled by the font), I should use a virtual function to get the zoom here
    const AreaSize new_size = GetSize(); // Need drawing size (including the zoom) to get the position (so ComputePosition must be called after ComputeZoom) 
    switch(xAnchor){
        case Anchor::Left:
            final_position.x = 0;
            break;
        case Anchor::Center:
            final_position.x = (m_parentSize.x-new_size.x)/2;
            break;
        case Anchor::Right:
            final_position.x = m_parentSize.x-new_size.x;
            break;
        default:
            std::cout << "Incorrect Anchor value for x axis\n"; // Error
    }
    switch(yAnchor){
        case Anchor::Top:
            final_position.y = 0;
            break;
        case Anchor::Center:
            final_position.y = (m_parentSize.y-new_size.y)/2;
            break;
        case Anchor::Bottom:
            final_position.y = m_parentSize.y-new_size.y;
            break;
        default:
            std::cout << "Incorrect Anchor value for y axis\n"; // Error
    }
    SetLocalPosition(final_position);
}

void UiElement::AddPadding(const Axis sourceAxis, const Axis paddingAxis, const float scale)
{
    ScreenPosition padding = {0,0};
    float v = 0.f;

    switch(sourceAxis){
        case Axis::Width:
            v = m_parentSize.x*scale;
            break;
        case Axis::Height:
            v = m_parentSize.y*scale;
            break;
    }

    switch(paddingAxis){
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

void UiElement::AddPadding(const Axis paddingAxis, const float scale)
{
    AddPadding(paddingAxis, paddingAxis, scale);
}

void UiController::SetRoot(UiElement* ui_root, const AreaSize initialSize)
{
    m_root = ui_root;
    m_root->SetParentSize(initialSize);
}

GameplayUiController::GameplayUiController(TextureController& textureController, const Camera& camera, const std::string& fontFilepath):
    m_dialogBox(textureController, "../assets/ui/box.png"), m_faceset(textureController, "../assets/ui/faceset.png"), 
    m_face(textureController, "../assets/ui/hunter_face.png"), m_textArea(textureController, fontFilepath)
{
    // For now, dialog box is the root of UiElement graph, with global position = local position
    // Technically, m_root sould be the camera viewport, but it's not a UiElement
    const AreaSize viewport_size = camera.GetViewport();
    SetRoot(&m_dialogBox, viewport_size);
    m_dialogBox.ComputeZoom(0.5f, Axis::Width);
    m_dialogBox.ComputePosition(Anchor::Center, Anchor::Bottom);
    m_dialogBox.AddPadding(Axis::Height, -0.05f);
    m_dialogBox.AddChild(&m_faceset);
    m_dialogBox.AddChild(&m_textArea);
    
    m_faceset.ComputeZoom(0.7f, Axis::Height);
    m_faceset.ComputePosition(Anchor::Left, Anchor::Center);
    m_faceset.AddPadding(Axis::Height, Axis::Width, 0.15f);
    m_faceset.AddChild(&m_face);

    m_face.ComputeZoom(0.8f, Axis::Width);
    m_face.ComputePosition(Anchor::Center, Anchor::Center);

    // Do not use ComputeZoom for TextArea. Text size is controlled by the font
    // Be sure to call TextArea::ComputePosition after generating the texture with SetText
    m_textArea.SetMaxWidth(0.75f); // Find another way to get this value
    m_textArea.SetText("Hello world ! This is an example of a long sentence to test how the text is wrapped by SDL_ttf...");
    m_textArea.ComputePosition(Anchor::Left, Anchor::Center);
    m_textArea.AddPadding(Axis::Width, 0.18f);

    m_dialogBox.UpdatePosition(camera.GetScreenOffset()); // Call UpdatePosition on the root UiElement
}

EditorUiController::EditorUiController(TextureController& textureController, const Camera& camera, const std::string& fontFilepath, const int selectedLayer):
    m_dialogBox(textureController, "../assets/ui/box.png"), m_textArea(textureController, fontFilepath), m_lastLayer(selectedLayer)
{
    const AreaSize viewport_size = camera.GetViewport();
    SetRoot(&m_dialogBox, viewport_size);
    m_dialogBox.ComputeZoom(0.3f, Axis::Width);
    m_dialogBox.ComputePosition(Anchor::Left, Anchor::Top);
    m_dialogBox.AddPadding(Axis::Width, 0.02f);
    m_dialogBox.AddPadding(Axis::Width, Axis::Height, 0.02f);
    m_dialogBox.AddChild(&m_textArea);
    
    m_textArea.SetMaxWidth(0.9f);
    m_textArea.SetText("Selected layer : " + std::to_string(m_lastLayer));
    m_textArea.ComputePosition(Anchor::Left, Anchor::Center);
    m_textArea.AddPadding(Axis::Width, 0.1f);

    m_dialogBox.UpdatePosition(camera.GetScreenOffset());
}

void EditorUiController::UpdateState(const int selectedLayer)
{
    if (selectedLayer != m_lastLayer){
        m_lastLayer = selectedLayer;
        m_textArea.SetText("Selected layer : " + std::to_string(m_lastLayer));
    }
}

BattleUiController::BattleUiController(TextureController& textureController, const Camera& camera, const std::string& fontFilepath):
    m_background(textureController, "../assets/battle/backgrounds/cavern.png"),
    m_player(textureController, "../assets/battle/werewolf.png"), m_enemy(textureController, "../assets/battle/bone_appetit.png"),
    m_playerBox(textureController, "../assets/ui/box.png"), m_enemyBox(textureController, "../assets/ui/box.png"),
    m_playerInfo(textureController, fontFilepath), m_enemyInfo(textureController, fontFilepath),
    m_mainBox(textureController, "../assets/ui/box.png")
{
    const float size = 0.2f; // Will be removed
    
    const AreaSize viewport_size = camera.GetViewport();
    SetRoot(&m_background, viewport_size);
    m_background.ComputeZoom(1.0f, Axis::Height);
    m_background.ComputePosition(Anchor::Left, Anchor::Top);
    m_background.AddChild(&m_player);
    m_background.AddChild(&m_enemy);
    m_background.AddChild(&m_mainBox);

    m_player.ComputeZoom(size, Axis::Width);
    m_player.ComputePosition(Anchor::Right, Anchor::Center);
    m_player.AddPadding(Axis::Width, -size);
    m_player.AddChild(&m_playerBox);
    
    m_enemy.ComputeZoom(size, Axis::Width);
    m_enemy.ComputePosition(Anchor::Left, Anchor::Center);
    m_enemy.AddPadding(Axis::Width, size);
    m_enemy.AddChild(&m_enemyBox);

    m_playerBox.ComputeZoom(0.8f, Axis::Width);
    m_playerBox.ComputePosition(Anchor::Right, Anchor::Bottom);
    m_playerBox.AddPadding(Axis::Width, size*2);
    m_playerBox.AddChild(&m_playerInfo);

    m_enemyBox.ComputeZoom(0.8f, Axis::Width);
    m_enemyBox.ComputePosition(Anchor::Right, Anchor::Bottom);
    m_enemyBox.AddPadding(Axis::Width, size*2);
    m_enemyBox.AddChild(&m_enemyInfo);

    m_playerInfo.SetMaxWidth(1.f);
    m_playerInfo.SetText("Howler");
    m_playerInfo.ComputePosition(Anchor::Left, Anchor::Center);
    m_playerInfo.AddPadding(Axis::Width, 0.1f);

    m_enemyInfo.SetMaxWidth(1.f);
    m_enemyInfo.SetText("Bone Appetit");
    m_enemyInfo.ComputePosition(Anchor::Left, Anchor::Center);
    m_enemyInfo.AddPadding(Axis::Width, 0.1f);

    m_mainBox.ComputeZoom(0.5f, Axis::Width);
    m_mainBox.ComputePosition(Anchor::Center, Anchor::Bottom);
    m_mainBox.AddPadding(Axis::Height, -0.05f);

    m_background.UpdatePosition(camera.GetScreenOffset());
}
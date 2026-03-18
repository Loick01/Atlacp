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
    m_text = text;
}

void TextArea::GenerateText()
{
    m_textureKey = m_text; // Will use something else than just text as texture key
    m_textureController.LoadTextureFromText(m_fontKey, m_textureKey, m_text, m_textureWidth, m_textureHeight, m_textColor, m_maxWidth);
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
            throw std::invalid_argument("Unknown axis value\n");
    }
}

void UiElement::ComputePosition(const Anchor xAnchor, const Anchor yAnchor)
{
    ScreenPosition final_position = {0,0};
    // If ComputePosition is called on a TextArea, the zoom here must be 1.0f (text size is handled by the font), I should use a virtual function to get the zoom here
    const AreaSize newSize = GetSize(); // Need drawing size (including the zoom) to get the position (so ComputePosition must be called after ComputeZoom) 
    switch(xAnchor){
        case Anchor::Left:
            final_position.x = 0;
            break;
        case Anchor::Center:
            final_position.x = (m_parentSize.x-newSize.x)/2;
            break;
        case Anchor::Right:
            final_position.x = m_parentSize.x-newSize.x;
            break;
        default:
            throw std::invalid_argument("Incorrect anchor value for x axis\n");
    }
    switch(yAnchor){
        case Anchor::Top:
            final_position.y = 0;
            break;
        case Anchor::Center:
            final_position.y = (m_parentSize.y-newSize.y)/2;
            break;
        case Anchor::Bottom:
            final_position.y = m_parentSize.y-newSize.y;
            break;
        default:
            throw std::invalid_argument("Incorrect anchor value for y axis\n");
    }
    SetLocalPosition(final_position);
}

void UiElement::AddPadding(const float scale, const Axis sourceAxis, const Axis paddingAxis)
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
        default:
            throw std::invalid_argument("Unknown source axis value\n");
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
            throw std::invalid_argument("Unknown padding axis value\n");
            break;
    }

    AddLocalPosition(padding);
}

void UiElement::AddPadding(const float scale, const Axis paddingAxis)
{
    AddPadding(scale, paddingAxis, paddingAxis);
}

void UiController::SetRoot(UiElement* ui_root, const AreaSize parentSize,
    const float zoomScale, const Axis zoomAxis, // ComputeZoom
    const Anchor xAnchor, const Anchor yAnchor, // ComputePosition
    const float paddingScale, const Axis sourceAxis, const Axis paddingAxis) // AddPadding
{
    m_root = ui_root;
    m_root->SetParentSize(parentSize);
    m_root->ComputeZoom(zoomScale, zoomAxis);
    m_root->ComputePosition(xAnchor, yAnchor);
    m_root->AddPadding(paddingScale, sourceAxis, paddingAxis);
}

void UiElement::AddChild(UiElement* child)
{
    m_childs.push_back(child);
}

void UiElement::MakeChild(UiElement* parent,
    const float zoomScale, const Axis zoomAxis,
    const Anchor xAnchor, const Anchor yAnchor,
    const float paddingScale, const Axis sourceAxis, const Axis paddingAxis)
{
    SetParentSize(parent->GetSize());
    ComputeZoom(zoomScale, zoomAxis);
    ComputePosition(xAnchor, yAnchor);
    AddPadding(paddingScale, sourceAxis, paddingAxis);
    parent->AddChild(this);
}

void TextArea::MakeChild(UiElement* parent,
    const float scaleWidth, const Axis zoomAxis,
    const Anchor xAnchor, const Anchor yAnchor,
    const float paddingScale, const Axis sourceAxis, const Axis paddingAxis)
{
    // Do not use ComputeZoom for TextArea. Text size is controlled by the font
    // Be sure to call TextArea::ComputePosition after generating the texture with GenerateText
    SetParentSize(parent->GetSize());
    SetMaxWidth(scaleWidth);
    GenerateText();
    ComputePosition(xAnchor, yAnchor);
    AddPadding(paddingScale, sourceAxis, paddingAxis);
    parent->AddChild(this);
}

GameplayUiController::GameplayUiController(TextureController& textureController, const Camera& camera, const std::string& fontFilepath):
    m_dialogBox(textureController, "../assets/ui/box.png"), m_faceset(textureController, "../assets/ui/faceset.png"), 
    m_face(textureController, "../assets/ui/hunter_face.png"), m_textArea(textureController, fontFilepath)
{
    // For now, dialog box is the root of UiElement graph, with global position = local position
    // Technically, m_root sould be the camera viewport, but it's not a UiElement
    const AreaSize viewport_size = camera.GetViewport();
    SetRoot(&m_dialogBox, viewport_size, 0.5f, Axis::Width, Anchor::Center, Anchor::Bottom, -0.05f, Axis::Height, Axis::Height);
    m_faceset.MakeChild(&m_dialogBox, 0.7f, Axis::Height, Anchor::Left, Anchor::Center, 0.15f, Axis::Height, Axis::Width);
    m_textArea.SetText("Hello world ! This is an example of a long sentence to test how the text is wrapped by SDL_ttf...");
    m_textArea.MakeChild(&m_dialogBox, 0.75f, Axis::Width, Anchor::Left, Anchor::Center, 0.18f, Axis::Width, Axis::Width);
    m_face.MakeChild(&m_faceset, 0.8f, Axis::Width, Anchor::Center, Anchor::Center, 0., Axis::Width, Axis::Width); // Axis::None ?

    m_dialogBox.UpdatePosition(camera.GetScreenOffset()); // Call UpdatePosition on the root UiElement
}

void GameplayUiController::Update()
{

}

EditorUiController::EditorUiController(TextureController& textureController, const Camera& camera, const std::string& fontFilepath):
    m_dialogBox(textureController, "../assets/ui/box.png"), m_textArea(textureController, fontFilepath), m_lastLayer(0) // lastLayer should be initialized with EditorEventInfo::selectedLayer ?
{
    const AreaSize viewport_size = camera.GetViewport();
    SetRoot(&m_dialogBox, viewport_size, 0.3f, Axis::Width, Anchor::Left, Anchor::Top, 0.02f, Axis::Width, Axis::Height);
    m_textArea.SetText("Selected layer : " + std::to_string(m_lastLayer));
    m_textArea.MakeChild(&m_dialogBox, 0.9f, Axis::Width, Anchor::Left, Anchor::Center, 0.1f, Axis::Width, Axis::Width);

    m_dialogBox.UpdatePosition(camera.GetScreenOffset());
}

void EditorUiController::SetEventInfo(const EditorEventInfo eventInfo)
{
    m_eventInfo = eventInfo;
}

void EditorUiController::Update()
{
    if (m_eventInfo.selectedLayer != m_lastLayer){
        m_lastLayer = m_eventInfo.selectedLayer;
        // Should merge the 2 lines below (with maybe calling ComputePosition) ?
        m_textArea.SetText("Selected layer : " + std::to_string(m_lastLayer));
        m_textArea.GenerateText();
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
    SetRoot(&m_background, viewport_size, 1.0f, Axis::Height, Anchor::Left, Anchor::Top, 0.f, Axis::Width, Axis::Width); // Axis::None for sourceAxis and paddingAxis ?

    m_player.MakeChild(&m_background, size, Axis::Width, Anchor::Right, Anchor::Center, -size, Axis::Width, Axis::Width);
    m_enemy.MakeChild(&m_background, size, Axis::Width, Anchor::Left, Anchor::Center, size, Axis::Width, Axis::Width);
    m_mainBox.MakeChild(&m_background, 0.5f, Axis::Width, Anchor::Center, Anchor::Bottom, -0.05f, Axis::Height, Axis::Height);

    m_playerBox.MakeChild(&m_player, 0.8f, Axis::Width, Anchor::Right, Anchor::Bottom, size*2, Axis::Width, Axis::Width);
    m_enemyBox.MakeChild(&m_enemy, 0.8f, Axis::Width, Anchor::Right, Anchor::Bottom, size*2, Axis::Width, Axis::Width);

    m_playerInfo.SetText("Howler");
    m_playerInfo.MakeChild(&m_playerBox, 1.f, Axis::Width, Anchor::Left, Anchor::Center, 0.1f, Axis::Width, Axis::Width);

    m_enemyInfo.SetText("Bone Appetit");
    m_enemyInfo.MakeChild(&m_enemyBox, 1.f, Axis::Width, Anchor::Left, Anchor::Center, 0.1f, Axis::Width, Axis::Width);

    m_background.UpdatePosition(camera.GetScreenOffset());
}

void BattleUiController::Update()
{

}
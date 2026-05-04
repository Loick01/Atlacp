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
    for (const std::unique_ptr<UiElement>& e : m_childs)
        e->DrawTexture();
}

void UiElement::UpdatePosition(const ScreenPosition parentPosition)
{
    SetScreenPosition(parentPosition + m_localPosition); // ScreenDrawable::m_position is the global position
    for (const std::unique_ptr<UiElement>& e : m_childs)
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

void UiElement::SetRelation(std::unique_ptr<UiElement>& child)
{
    m_childs.push_back(std::move(child));
}

void UiElement::AddChild(std::unique_ptr<UiElement> child,
    const float scale, const Axis zoomAxis,
    const Anchor xAnchor, const Anchor yAnchor,
    const float paddingScale, const Axis sourceAxis, const Axis paddingAxis)
{
    child->SetParentSize(GetSize());
    child->Compute(scale, zoomAxis, xAnchor, yAnchor, paddingScale, sourceAxis, paddingAxis);
    SetRelation(child);
}

void UiElement::Compute(
    const float scale, const Axis zoomAxis,
    const Anchor xAnchor, const Anchor yAnchor,
    const float paddingScale, const Axis sourceAxis, const Axis paddingAxis)
{
    ComputeZoom(scale, zoomAxis);
    ComputePosition(xAnchor, yAnchor);
    AddPadding(paddingScale, sourceAxis, paddingAxis);
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

void TextArea::Compute(
    const float scale, const Axis zoomAxis,
    const Anchor xAnchor, const Anchor yAnchor,
    const float paddingScale, const Axis sourceAxis, const Axis paddingAxis)
{
    // Do not use ComputeZoom for TextArea. Text size is controlled by the font
    // Be sure to call TextArea::ComputePosition after generating the texture with GenerateText
    SetMaxWidth(scale);
    GenerateText();
    ComputePosition(xAnchor, yAnchor);
    AddPadding(paddingScale, sourceAxis, paddingAxis);
}

void UiController::Draw() const
{
    m_root->DrawTexture();
}

void UiController::SetRoot(std::unique_ptr<UiElement>& ui_root, const AreaSize parentSize,
    const float zoomScale, const Axis zoomAxis, // ComputeZoom
    const Anchor xAnchor, const Anchor yAnchor, // ComputePosition
    const float paddingScale, const Axis sourceAxis, const Axis paddingAxis) // AddPadding
{
    m_root = std::move(ui_root); // ?
    m_root->SetParentSize(parentSize);
    m_root->ComputeZoom(zoomScale, zoomAxis);
    m_root->ComputePosition(xAnchor, yAnchor);
    m_root->AddPadding(paddingScale, sourceAxis, paddingAxis);
}

void UiController::OpenDialogBox()
{
    // SetRoot(&m_frame, viewport_size, 0.5f, Axis::Width, Anchor::Center, Anchor::Bottom, -0.05f, Axis::Height, Axis::Height);
    // m_faceset.MakeChild(&m_frame, 0.7f, Axis::Height, Anchor::Left, Anchor::Center, 0.15f, Axis::Height, Axis::Width);
    // m_textArea.SetText("Hello world ! This is an example of a long sentence to test how the text is wrapped by SDL_ttf...");
    // m_textArea.MakeChild(&m_frame, 0.75f, Axis::Width, Anchor::Left, Anchor::Center, 0.18f, Axis::Width, Axis::Width);
    // m_face.MakeChild(&m_faceset, 0.8f, Axis::Width, Anchor::Center, Anchor::Center, 0., Axis::Width, Axis::Width); // Axis::None ?
}

GameplayUiController::GameplayUiController(TextureController& textureController, const Camera& camera, const std::string& fontFilepath)
{
    std::unique_ptr<UiElement> frame = std::make_unique<UiElement>(textureController, "../assets/ui/box.png");
    m_elements["frame"] = frame.get();
    std::unique_ptr<UiElement> faceset = std::make_unique<UiElement>(textureController, "../assets/ui/faceset.png");
    m_elements["faceset"] = faceset.get();
    std::unique_ptr<UiElement> face = std::make_unique<UiElement>(textureController, "../assets/ui/hunter_face.png");
    m_elements["face"] = face.get();
    std::unique_ptr<TextArea> textArea = std::make_unique<TextArea>(textureController, fontFilepath);
    m_elements["textArea"] = textArea.get();
    // For now, dialog box is the root of UiElement graph, with global position = local position
    // Technically, m_root sould be the camera viewport, but it's not a UiElement

    const AreaSize viewport_size = camera.GetViewport();
    SetRoot(frame, viewport_size, 0.5f, Axis::Width, Anchor::Center, Anchor::Bottom, -0.05f, Axis::Height, Axis::Height);

    m_root->AddChild(std::move(faceset), 0.7f, Axis::Height, Anchor::Left, Anchor::Center, 0.15f, Axis::Height, Axis::Width);

    textArea->SetText("Hello world ! This is an example of a long sentence to test how the text is wrapped by SDL_ttf...");
    m_root->AddChild(std::move(textArea), 0.75f, Axis::Width, Anchor::Left, Anchor::Center, 0.18f, Axis::Width, Axis::Width);

    m_elements["faceset"]->AddChild(std::move(face), 0.8f, Axis::Width, Anchor::Center, Anchor::Center, 0., Axis::Width, Axis::Width); // Axis::None ?

    m_root->UpdatePosition(camera.GetScreenOffset()); // Call UpdatePosition on the root UiElement
}

void GameplayUiController::Update()
{

}

EditorUiController::EditorUiController(TextureController& textureController, const Camera& camera, const std::string& fontFilepath):
    m_lastLayer(0) // lastLayer should be initialized with EditorEventState::selectedLayer ?
{
    std::unique_ptr<UiElement> frame = std::make_unique<UiElement>(textureController, "../assets/ui/box.png");
    m_elements["frame"] = frame.get();
    std::unique_ptr<TextArea> textArea = std::make_unique<TextArea>(textureController, fontFilepath);
    m_elements["textArea"] = textArea.get();

    const AreaSize viewport_size = camera.GetViewport();
    SetRoot(frame, viewport_size, 0.3f, Axis::Width, Anchor::Left, Anchor::Top, 0.02f, Axis::Width, Axis::Height);

    textArea->SetText("Selected layer : " + std::to_string(m_lastLayer));
    m_root->AddChild(std::move(textArea), 0.9f, Axis::Width, Anchor::Left, Anchor::Center, 0.1f, Axis::Width, Axis::Width);

    m_root->UpdatePosition(camera.GetScreenOffset()); // Call UpdatePosition on the root UiElement
}

void EditorUiController::Update()
{
    // if (m_eventState.selectedLayer != m_lastLayer){
    //     m_lastLayer = m_eventState.selectedLayer;
    //     // Should merge the 2 lines below (with maybe calling ComputePosition) ?
    //     m_textArea.SetText("Selected layer : " + std::to_string(m_lastLayer));
    //     m_textArea.GenerateText();
    // } 
}

BattleUiController::BattleUiController(TextureController& textureController, const Camera& camera, const std::string& fontFilepath)
{
    std::unique_ptr<UiElement> background = std::make_unique<UiElement>(textureController, "../assets/battle/backgrounds/cavern.png");
    m_elements["background"] = background.get();
    std::unique_ptr<UiElement> actorASprite = std::make_unique<UiElement>(textureController, "../assets/battle/werewolf.png");
    m_elements["actorASprite"] = actorASprite.get();
    std::unique_ptr<UiElement> actorBSprite = std::make_unique<UiElement>(textureController, "../assets/battle/bone_appetit.png");
    m_elements["actorBSprite"] = actorBSprite.get();
    std::unique_ptr<UiElement> actorABox = std::make_unique<UiElement>(textureController, "../assets/ui/box.png");
    m_elements["actorABox"] = actorABox.get();
    std::unique_ptr<UiElement> actorBBox = std::make_unique<UiElement>(textureController, "../assets/ui/box.png");
    m_elements["actorBBox"] = actorBBox.get();
    std::unique_ptr<UiElement> mainBox = std::make_unique<UiElement>(textureController, "../assets/ui/box.png");
    m_elements["mainBox"] = mainBox.get();
    std::unique_ptr<TextArea> actorAInfo = std::make_unique<TextArea>(textureController, fontFilepath);
    m_elements["actorAInfo"] = actorAInfo.get();
    std::unique_ptr<TextArea> actorBInfo = std::make_unique<TextArea>(textureController, fontFilepath);
    m_elements["actorBInfo"] = actorBInfo.get();

    const float size = 0.2f; // Will be removed
    const AreaSize viewport_size = camera.GetViewport();
    SetRoot(background, viewport_size, 1.0f, Axis::Height, Anchor::Left, Anchor::Top, 0.f, Axis::Width, Axis::Width); // Axis::None for sourceAxis and paddingAxis ?

    m_root->AddChild(std::move(actorASprite), size, Axis::Width, Anchor::Right, Anchor::Center, -size, Axis::Width, Axis::Width);
    m_root->AddChild(std::move(actorBSprite), size, Axis::Width, Anchor::Left, Anchor::Center, size, Axis::Width, Axis::Width);
    m_root->AddChild(std::move(mainBox), 0.5f, Axis::Width, Anchor::Center, Anchor::Bottom, -0.05f, Axis::Height, Axis::Height);

    m_elements["actorASprite"]->AddChild(std::move(actorABox), 0.8f, Axis::Width, Anchor::Right, Anchor::Bottom, size*2, Axis::Width, Axis::Width);
    m_elements["actorBSprite"]->AddChild(std::move(actorBBox), 0.8f, Axis::Width, Anchor::Right, Anchor::Bottom, size*2, Axis::Width, Axis::Width);

    actorAInfo->SetText("Howler"); // Will be removed
    m_elements["actorABox"]->AddChild(std::move(actorAInfo), 1.f, Axis::Width, Anchor::Left, Anchor::Center, 0.1f, Axis::Width, Axis::Width);

    actorBInfo->SetText("Bone Appetit"); // Will be removed
    m_elements["actorBBox"]->AddChild(std::move(actorBInfo), 1.f, Axis::Width, Anchor::Left, Anchor::Center, 0.1f, Axis::Width, Axis::Width);

    m_root->UpdatePosition(camera.GetScreenOffset()); // Call UpdatePosition on the root UiElement
}

void BattleUiController::SetActorAName(const std::string name)
{
    // m_actorAInfo.SetText(name);
    // m_actorAInfo.MakeChild(&m_actorABox, 1.f, Axis::Width, Anchor::Left, Anchor::Center, 0.1f, Axis::Width, Axis::Width);
    // m_actorAInfo.UpdatePosition(m_actorABox.GetScreenPosition());
}

void BattleUiController::SetActorBName(const std::string name)
{
    // m_actorBInfo.SetText(name);
    // m_actorBInfo.MakeChild(&m_actorBBox, 1.f, Axis::Width, Anchor::Left, Anchor::Center, 0.1f, Axis::Width, Axis::Width);
    // m_actorBInfo.UpdatePosition(m_actorBBox.GetScreenPosition());
}

void BattleUiController::Update()
{

}
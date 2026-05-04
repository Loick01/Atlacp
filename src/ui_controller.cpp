#include "ui_controller.hpp"

void UiController::Draw() const
{
    m_root->DrawTexture();
}

void UiController::BuildRoot(std::unique_ptr<UiElement> ui_root, const AreaSize parentSize,
    const float zoomScale, const Axis zoomAxis, // ComputeZoom
    const Anchor xAnchor, const Anchor yAnchor, // ComputePosition
    const float paddingScale, const Axis sourceAxis, const Axis paddingAxis) // AddPadding
{
    m_root = std::move(ui_root); 
    m_root->SetParentSize(parentSize);
    m_root->ComputeZoom(zoomScale, zoomAxis);
    m_root->ComputePosition(xAnchor, yAnchor);
    m_root->AddPadding(paddingScale, sourceAxis, paddingAxis);
}

// void UiController::OpenDialogBox()
// {
//     // BuildRoot(&m_frame, viewport_size, 0.5f, Axis::Width, Anchor::Center, Anchor::Bottom, -0.05f, Axis::Height, Axis::Height);
//     // m_faceset.MakeChild(&m_frame, 0.7f, Axis::Height, Anchor::Left, Anchor::Center, 0.15f, Axis::Height, Axis::Width);
//     // m_textArea.SetText("Hello world ! This is an example of a long sentence to test how the text is wrapped by SDL_ttf...");
//     // m_textArea.MakeChild(&m_frame, 0.75f, Axis::Width, Anchor::Left, Anchor::Center, 0.18f, Axis::Width, Axis::Width);
//     // m_face.MakeChild(&m_faceset, 0.8f, Axis::Width, Anchor::Center, Anchor::Center, 0., Axis::Width, Axis::Width); // Axis::None ?
// }

UiElement* UiController::GetElement(ElementKey key)
{
    if (m_elements.find(key) == m_elements.end())
        throw std::runtime_error("This Ui element can't be found : " + key); // Should return nullptr instead ?
    return m_elements[key];
}

void UiController::UpdateText(const ElementKey& key, const std::string& newText)
{
    TextArea* textArea = static_cast<TextArea*>(GetElement(key));
    textArea->SetText(newText);
    textArea->GenerateText();
    // This is not enough, ComputeFinal() must be called on textArea here, but I need padding, zoom, anchors, etc.
    // Also need to call UpdatePosition() ?

    //textArea->UpdatePosition();
    // m_actorAInfo.SetText(name);
    // m_actorAInfo.MakeChild(&m_actorABox, 1.f, Axis::Width, Anchor::Left, Anchor::Center, 0.1f, Axis::Width, Axis::Width);
    // m_actorAInfo.UpdatePosition(m_actorABox.GetScreenPosition());
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
    BuildRoot(std::move(frame), viewport_size, 0.5f, Axis::Width, Anchor::Center, Anchor::Bottom, -0.05f, Axis::Height, Axis::Height);

    m_root->BuildChild(std::move(faceset), 0.7f, Axis::Height, Anchor::Left, Anchor::Center, 0.15f, Axis::Height, Axis::Width);

    textArea->SetText("Hello world ! This is an example of a long sentence to test how the text is wrapped by SDL_ttf...");
    m_root->BuildChild(std::move(textArea), 0.75f, Axis::Width, Anchor::Left, Anchor::Center, 0.18f, Axis::Width, Axis::Width);

    m_elements["faceset"]->BuildChild(std::move(face), 0.8f, Axis::Width, Anchor::Center, Anchor::Center, 0., Axis::Width, Axis::Width); // Axis::None ?

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
    BuildRoot(std::move(frame), viewport_size, 0.3f, Axis::Width, Anchor::Left, Anchor::Top, 0.02f, Axis::Width, Axis::Height);

    textArea->SetText("Selected layer : " + std::to_string(m_lastLayer));
    m_root->BuildChild(std::move(textArea), 0.9f, Axis::Width, Anchor::Left, Anchor::Center, 0.1f, Axis::Width, Axis::Width);

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
    BuildRoot(std::move(background), viewport_size, 1.0f, Axis::Height, Anchor::Left, Anchor::Top, 0.f, Axis::Width, Axis::Width); // Axis::None for sourceAxis and paddingAxis ?

    m_root->BuildChild(std::move(actorASprite), size, Axis::Width, Anchor::Right, Anchor::Center, -size, Axis::Width, Axis::Width);
    m_root->BuildChild(std::move(actorBSprite), size, Axis::Width, Anchor::Left, Anchor::Center, size, Axis::Width, Axis::Width);
    m_root->BuildChild(std::move(mainBox), 0.5f, Axis::Width, Anchor::Center, Anchor::Bottom, -0.05f, Axis::Height, Axis::Height);

    m_elements["actorASprite"]->BuildChild(std::move(actorABox), 0.8f, Axis::Width, Anchor::Right, Anchor::Bottom, size*2, Axis::Width, Axis::Width);
    m_elements["actorBSprite"]->BuildChild(std::move(actorBBox), 0.8f, Axis::Width, Anchor::Right, Anchor::Bottom, size*2, Axis::Width, Axis::Width);
    
    actorAInfo->SetText("Howler"); // Will be removed
    m_elements["actorABox"]->BuildChild(std::move(actorAInfo), 1.f, Axis::Width, Anchor::Left, Anchor::Center, 0.1f, Axis::Width, Axis::Width);

    actorBInfo->SetText("Bone Appetit"); // Will be removed
    m_elements["actorBBox"]->BuildChild(std::move(actorBInfo), 1.f, Axis::Width, Anchor::Left, Anchor::Center, 0.1f, Axis::Width, Axis::Width);

    m_root->UpdatePosition(camera.GetScreenOffset()); // Call UpdatePosition on the root UiElement
}

void BattleUiController::Update()
{

}
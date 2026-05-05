#include "ui_controller.hpp"

void UiController::Draw() const
{
    m_root->DrawTexture();
}

void UiController::BuildRoot(std::unique_ptr<UiElement> ui_root, 
    const AreaSize parentSize, const ScreenPosition parentPosition)
{
    m_root = std::move(ui_root); 
    m_root->SetParentSize(parentSize);
    m_root->SetParentPosition(parentPosition);
    const UiParams& params = m_root->GetParams();
    m_root->ComputeZoom(params.scale, params.zoomAxis);
    m_root->ComputePosition(params.xAnchor, params.yAnchor);
    m_root->AddPadding(params.paddingScale, params.sourceAxis, params.paddingAxis);
}

// void UiController::OpenDialogBox()
// {
//     // BuildRoot(&m_frame, viewportSize, 0.5f, Axis::Width, Anchor::Center, Anchor::BottomIn, -0.05f, Axis::Height, Axis::Height);
//     // m_faceset.MakeChild(&m_frame, 0.7f, Axis::Height, Anchor::LeftIn, Anchor::Center, 0.15f, Axis::Height, Axis::Width);
//     // m_textArea.SetText("Hello world ! This is an example of a long sentence to test how the text is wrapped by SDL_ttf...");
//     // m_textArea.MakeChild(&m_frame, 0.75f, Axis::Width, Anchor::LeftIn, Anchor::Center, 0.18f, Axis::Width, Axis::Width);
//     // m_face.MakeChild(&m_faceset, 0.8f, Axis::Width, Anchor::Center, Anchor::Center, 0., Axis::Width, Axis::Width); // Axis::None ?
// }

UiElement* UiController::GetElement(const ElementKey& key)
{
    if (m_elements.find(key) == m_elements.end())
        throw std::runtime_error("This Ui element can't be found : " + key); // Should return nullptr instead ?
    return m_elements[key];
}

// float UiController::GetPartialElementSizeOnAxis(const ElementKey& key, const float amount, const Axis axis)
// {
//     const UiElement* element = GetElement(key); // ComputeFinal must have been called on this UiElement
//     switch(axis) {
//         case Axis::Width : {
//             return element->GetSize().x * amount;
//             break;
//         }
//         case Axis::Height : {
//             return element->GetSize().y * amount;
//             break;
//         }
//         default : {
//             throw std::invalid_argument("Unknown axis value\n");
//         }
//     }
// }

void UiController::UpdateText(const ElementKey& key, const std::string& newText)
{
    TextArea* textArea = static_cast<TextArea*>(GetElement(key));
    textArea->SetText(newText);
    textArea->GenerateText();
    // textArea->ComputeFinal(); // I would need anchor, scale, padding, etc.
    // textArea->UpdatePosition();
}

GameplayUiController::GameplayUiController(TextureController& textureController, const Camera& camera, const std::string& fontFilepath)
{
    std::unique_ptr<UiElement> frame = std::make_unique<UiElement>(textureController, "../assets/ui/box.png");
    m_elements["frame"] = frame.get();
    std::unique_ptr<UiElement> faceset = std::make_unique<UiElement>(textureController, "../assets/ui/faceset.png");
    m_elements["faceset"] = faceset.get();
    std::unique_ptr<UiElement> face = std::make_unique<UiElement>(textureController, "../assets/ui/hunter_face.png");
    m_elements["face"] = face.get();
    std::unique_ptr<TextArea> boxText = std::make_unique<TextArea>(textureController, fontFilepath);
    m_elements["boxText"] = boxText.get();
    // For now, dialog box is the root of UiElement graph, with global position = local position
    // Technically, m_root sould be the camera viewport, but it's not a UiElement

    const AreaSize viewportSize = camera.GetViewport(); // Parent size
    const ScreenPosition viewportPosition = camera.GetScreenOffset(); // Parent position
    UiParams& frameParams = frame->GetParams();
    frameParams.scale = 0.5f;
    frameParams.zoomAxis = Axis::Width;
    frameParams.xAnchor = Anchor::Center;
    frameParams.yAnchor = Anchor::BottomIn;
    frameParams.paddingScale = -0.05f;
    frameParams.sourceAxis = Axis::Height;
    frameParams.paddingAxis = Axis::Height;
    BuildRoot(std::move(frame), viewportSize, viewportPosition);

    UiParams& facesetParams = faceset->GetParams();
    facesetParams.scale = 0.7f;
    facesetParams.zoomAxis = Axis::Height;
    facesetParams.xAnchor = Anchor::LeftIn;
    facesetParams.yAnchor = Anchor::Center;
    facesetParams.paddingScale = 0.15f;
    facesetParams.sourceAxis = Axis::Height;
    facesetParams.paddingAxis = Axis::Width;
    m_root->BuildChild(std::move(faceset));

    boxText->SetText("Hello world ! This is an example of a long sentence to test how the text is wrapped by SDL_ttf...");
    UiParams& boxTextParams = boxText->GetParams();
    boxTextParams.scale = 0.75f;
    boxTextParams.zoomAxis = Axis::Width; // Unused because TextArea do not use zoom
    boxTextParams.xAnchor = Anchor::RightOut;
    boxTextParams.yAnchor = Anchor::Center;
    boxTextParams.paddingScale = 0.18f;
    boxTextParams.sourceAxis = Axis::Width;
    boxTextParams.paddingAxis = Axis::Width;
    m_elements["faceset"]->BuildChild(std::move(boxText));

    UiParams& faceParams = face->GetParams();
    faceParams.scale = 0.8f;
    faceParams.zoomAxis = Axis::Width;
    faceParams.xAnchor = Anchor::Center;
    faceParams.yAnchor = Anchor::Center;
    faceParams.paddingScale = 0.f;
    faceParams.sourceAxis = Axis::Width; // Axis::None ?
    faceParams.paddingAxis = Axis::Width; // Axis::None ?
    m_elements["faceset"]->BuildChild(std::move(face));

    m_root->UpdatePosition(); // Call UpdatePosition on the root UiElement
}

void GameplayUiController::Update()
{

}

EditorUiController::EditorUiController(TextureController& textureController, const Camera& camera, const std::string& fontFilepath):
    m_lastLayer(0) // lastLayer should be initialized with EditorEventState::selectedLayer ?
{
    std::unique_ptr<UiElement> frame = std::make_unique<UiElement>(textureController, "../assets/ui/box.png");
    m_elements["frame"] = frame.get();
    std::unique_ptr<TextArea> boxText = std::make_unique<TextArea>(textureController, fontFilepath);
    m_elements["boxText"] = boxText.get();

    const AreaSize viewportSize = camera.GetViewport(); // Parent size
    const ScreenPosition viewportPosition = camera.GetScreenOffset(); // Parent position
    UiParams& frameParams = frame->GetParams();
    frameParams.scale = 0.3f;
    frameParams.zoomAxis = Axis::Width;
    frameParams.xAnchor = Anchor::LeftIn;
    frameParams.yAnchor = Anchor::TopIn;
    frameParams.paddingScale = 0.02f;
    frameParams.sourceAxis = Axis::Width;
    frameParams.paddingAxis = Axis::Height;
    BuildRoot(std::move(frame), viewportSize, viewportPosition);

    boxText->SetText("Selected layer : " + std::to_string(m_lastLayer));
    UiParams& boxTextParams = boxText->GetParams();
    boxTextParams.scale = 0.9f;
    boxTextParams.zoomAxis = Axis::Width; // Unused because TextArea do not use zoom
    boxTextParams.xAnchor = Anchor::LeftIn;
    boxTextParams.yAnchor = Anchor::Center;
    boxTextParams.paddingScale = 0.1f;
    boxTextParams.sourceAxis = Axis::Width;
    boxTextParams.paddingAxis = Axis::Width;
    m_root->BuildChild(std::move(boxText));

    m_root->UpdatePosition(); // Call UpdatePosition on the root UiElement
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
    std::unique_ptr<TextArea> actorAName = std::make_unique<TextArea>(textureController, fontFilepath);
    m_elements["actorAName"] = actorAName.get();
    std::unique_ptr<TextArea> actorBName = std::make_unique<TextArea>(textureController, fontFilepath);
    m_elements["actorBName"] = actorBName.get();
    std::unique_ptr<TextArea> actorAHealth = std::make_unique<TextArea>(textureController, fontFilepath);
    m_elements["actorAHealth"] = actorAHealth.get();
    std::unique_ptr<TextArea> actorBHealth = std::make_unique<TextArea>(textureController, fontFilepath);
    m_elements["actorBHealth"] = actorBHealth.get();

    const float size = 0.2f; // Will be removed
    const AreaSize viewportSize = camera.GetViewport(); // Parent size
    const ScreenPosition viewportPosition = camera.GetScreenOffset(); // Parent position
    UiParams& backgroundParams = background->GetParams();
    backgroundParams.scale = 1.f;
    backgroundParams.zoomAxis = Axis::Height;
    backgroundParams.xAnchor = Anchor::LeftIn;
    backgroundParams.yAnchor = Anchor::TopIn;
    backgroundParams.paddingScale = 0.f; // No padding
    backgroundParams.sourceAxis = Axis::Width; // Axis::None ?
    backgroundParams.paddingAxis = Axis::Width; // Axis::None ?
    BuildRoot(std::move(background), viewportSize, viewportPosition); 

    UiParams& actorASpriteParams = actorASprite->GetParams();
    actorASpriteParams.scale = size;
    actorASpriteParams.zoomAxis = Axis::Width;
    actorASpriteParams.xAnchor = Anchor::RightIn;
    actorASpriteParams.yAnchor = Anchor::Center;
    actorASpriteParams.paddingScale = -size;
    actorASpriteParams.sourceAxis = Axis::Width;
    actorASpriteParams.paddingAxis = Axis::Width;
    m_root->BuildChild(std::move(actorASprite));

    UiParams& actorBSpriteParams = actorBSprite->GetParams();
    actorBSpriteParams.scale = size;
    actorBSpriteParams.zoomAxis = Axis::Width;
    actorBSpriteParams.xAnchor = Anchor::LeftIn;
    actorBSpriteParams.yAnchor = Anchor::Center;
    actorBSpriteParams.paddingScale = size;
    actorBSpriteParams.sourceAxis = Axis::Width;
    actorBSpriteParams.paddingAxis = Axis::Width;
    m_root->BuildChild(std::move(actorBSprite));

    UiParams& mainBoxParams = mainBox->GetParams();
    mainBoxParams.scale = 0.5f;
    mainBoxParams.zoomAxis = Axis::Width;
    mainBoxParams.xAnchor = Anchor::Center;
    mainBoxParams.yAnchor = Anchor::BottomIn;
    mainBoxParams.paddingScale = -0.05f;
    mainBoxParams.sourceAxis = Axis::Height;
    mainBoxParams.paddingAxis = Axis::Height;
    m_root->BuildChild(std::move(mainBox));

    UiParams& actorABoxParams = actorABox->GetParams();
    actorABoxParams.scale = 1.f;
    actorABoxParams.zoomAxis = Axis::Width;
    actorABoxParams.xAnchor = Anchor::RightIn;
    actorABoxParams.yAnchor = Anchor::BottomIn;
    actorABoxParams.paddingScale = size*3;
    actorABoxParams.sourceAxis = Axis::Width;
    actorABoxParams.paddingAxis = Axis::Width;
    m_elements["actorASprite"]->BuildChild(std::move(actorABox));

    UiParams& actorBBoxParams = actorBBox->GetParams();
    actorBBoxParams.scale = 1.f;
    actorBBoxParams.zoomAxis = Axis::Width;
    actorBBoxParams.xAnchor = Anchor::RightIn;
    actorBBoxParams.yAnchor = Anchor::BottomIn;
    actorBBoxParams.paddingScale = size*3;
    actorBBoxParams.sourceAxis = Axis::Width;
    actorBBoxParams.paddingAxis = Axis::Width;
    m_elements["actorBSprite"]->BuildChild(std::move(actorBBox));
    
    UiParams& actorANameParams = actorAName->GetParams();
    actorANameParams.scale = 1.f;
    actorANameParams.zoomAxis = Axis::Width;
    actorANameParams.xAnchor = Anchor::LeftIn;
    actorANameParams.yAnchor = Anchor::Center;
    actorANameParams.paddingScale = 0.05f;
    actorANameParams.sourceAxis = Axis::Width;
    actorANameParams.paddingAxis = Axis::Width;
    m_elements["actorABox"]->BuildChild(std::move(actorAName));
    
    UiParams& actorAHealthParams = actorAHealth->GetParams();
    actorAHealthParams.scale = 1.f;
    actorAHealthParams.zoomAxis = Axis::Width;
    actorAHealthParams.xAnchor = Anchor::LeftIn;
    actorAHealthParams.yAnchor = Anchor::Center;
    actorAHealthParams.paddingScale = 1.f;
    actorAHealthParams.sourceAxis = Axis::Height;
    actorAHealthParams.paddingAxis = Axis::Height;
    m_elements["actorAName"]->BuildChild(std::move(actorAHealth));

    UiParams& actorBNameParams = actorBName->GetParams();
    actorBNameParams.scale = 1.f;
    actorBNameParams.zoomAxis = Axis::Width;
    actorBNameParams.xAnchor = Anchor::LeftIn;
    actorBNameParams.yAnchor = Anchor::Center;
    actorBNameParams.paddingScale = 0.05f;
    actorBNameParams.sourceAxis = Axis::Width;
    actorBNameParams.paddingAxis = Axis::Width;
    m_elements["actorBBox"]->BuildChild(std::move(actorBName));

    UiParams& actorBHealthParams = actorBHealth->GetParams();
    actorBHealthParams.scale = 1.f;
    actorBHealthParams.zoomAxis = Axis::Width;
    actorBHealthParams.xAnchor = Anchor::LeftIn;
    actorBHealthParams.yAnchor = Anchor::Center;
    actorBHealthParams.paddingScale = 1.f;
    actorBHealthParams.sourceAxis = Axis::Height;
    actorBHealthParams.paddingAxis = Axis::Height;
    m_elements["actorBName"]->BuildChild(std::move(actorBHealth));

    m_root->UpdatePosition(); // Call UpdatePosition on the root UiElement
}

void BattleUiController::Update()
{

}
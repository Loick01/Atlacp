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
    m_root->ComputeZoom(params.scale, params.scaleAxis);
    m_root->ComputePosition(params.xAnchor, params.yAnchor);
    m_root->SetPadding(params.xPadding, params.yPadding);
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

float UiController::GetPartialElementSizeOnAxis(const ElementKey& key, const Axis axis, const float amount)
{
    const UiElement* element = GetElement(key); // ComputeFinal must have been called on this UiElement
    switch(axis) {
        case Axis::Width : {
            return element->GetSize().x * amount;
            break;
        }
        case Axis::Height : {
            return element->GetSize().y * amount;
            break;
        }
        default : {
            throw std::invalid_argument("Unknown axis value\n");
        }
    }
}

void UiController::UpdateText(const ElementKey& key, const std::string& newText)
{
    TextArea* textArea = static_cast<TextArea*>(GetElement(key));
    textArea->SetText(newText);
    textArea->GenerateText();
    // textArea->ComputeFinal(); // I would need anchor, scale, padding, etc.
    // textArea->UpdatePosition();
}

GameplayUiController::GameplayUiController(TextureController& textureController, const std::string& fontFilepath,
    const AreaSize viewportSize, const ScreenPosition viewportPosition)
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

    UiParams& frameParams = frame->GetParams();
    frameParams.scale = 0.5f;
    frameParams.scaleAxis = Axis::Width;
    frameParams.xAnchor = Anchor::Center;
    frameParams.yAnchor = Anchor::BottomIn;
    frameParams.yPadding = viewportSize.y * -0.05f;
    BuildRoot(std::move(frame), viewportSize, viewportPosition);

    UiParams& facesetParams = faceset->GetParams();
    facesetParams.scale = 0.7f;
    facesetParams.scaleAxis = Axis::Height;
    facesetParams.xAnchor = Anchor::LeftIn;
    facesetParams.yAnchor = Anchor::Center;
    facesetParams.xPadding = GetPartialElementSizeOnAxis("frame", Axis::Height, 0.15f); // Should not access to root element with its key ?
    m_root->BuildChild(std::move(faceset));

    boxText->SetText("Hello world ! This is an example of a long sentence to test how the text is wrapped by SDL_ttf...");
    UiParams& boxTextParams = boxText->GetParams();
    boxTextParams.scale = 0.75f; // TODO
    boxTextParams.xAnchor = Anchor::RightOut;
    boxTextParams.yAnchor = Anchor::Center;
    boxTextParams.xPadding = GetPartialElementSizeOnAxis("faceset", Axis::Width, 0.2f);
    m_elements["faceset"]->BuildChild(std::move(boxText));

    UiParams& faceParams = face->GetParams();
    faceParams.scale = 0.8f;
    faceParams.scaleAxis = Axis::Width;
    faceParams.xAnchor = Anchor::Center;
    faceParams.yAnchor = Anchor::Center;
    m_elements["faceset"]->BuildChild(std::move(face));

    m_root->UpdatePosition(); // Call UpdatePosition on the root UiElement
}

void GameplayUiController::Update()
{

}

EditorUiController::EditorUiController(TextureController& textureController, const std::string& fontFilepath,
    const AreaSize viewportSize, const ScreenPosition viewportPosition):
    m_lastLayer(0) // lastLayer should be initialized with EditorEventState::selectedLayer ?
{
    std::unique_ptr<UiElement> frame = std::make_unique<UiElement>(textureController, "../assets/ui/box.png");
    m_elements["frame"] = frame.get();
    std::unique_ptr<TextArea> boxText = std::make_unique<TextArea>(textureController, fontFilepath);
    m_elements["boxText"] = boxText.get();

    UiParams& frameParams = frame->GetParams();
    frameParams.scale = 0.3f;
    frameParams.scaleAxis = Axis::Width;
    frameParams.xAnchor = Anchor::LeftIn;
    frameParams.yAnchor = Anchor::TopIn;
    frameParams.yPadding = viewportSize.x * 0.02f;
    BuildRoot(std::move(frame), viewportSize, viewportPosition);

    boxText->SetText("Selected layer : " + std::to_string(m_lastLayer));
    UiParams& boxTextParams = boxText->GetParams();
    boxTextParams.scale = 0.9f;
    boxTextParams.xAnchor = Anchor::LeftIn;
    boxTextParams.yAnchor = Anchor::Center;
    boxTextParams.xPadding = GetPartialElementSizeOnAxis("frame", Axis::Width, 0.1f); // Should not access to root element with its key ?
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

BattleUiController::BattleUiController(TextureController& textureController, const std::string& fontFilepath,
    const AreaSize viewportSize, const ScreenPosition viewportPosition)
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
    UiParams& backgroundParams = background->GetParams();
    backgroundParams.scale = 1.f;
    backgroundParams.scaleAxis = Axis::Height;
    backgroundParams.xAnchor = Anchor::LeftIn;
    backgroundParams.yAnchor = Anchor::TopIn;
    BuildRoot(std::move(background), viewportSize, viewportPosition); 

    UiParams& actorASpriteParams = actorASprite->GetParams();
    actorASpriteParams.scale = size;
    actorASpriteParams.scaleAxis = Axis::Width;
    actorASpriteParams.xAnchor = Anchor::RightIn;
    actorASpriteParams.yAnchor = Anchor::Center;
    actorASpriteParams.xPadding = GetPartialElementSizeOnAxis("background", Axis::Width, -size); // Should not access to root element with its key ?
    m_root->BuildChild(std::move(actorASprite));

    UiParams& actorBSpriteParams = actorBSprite->GetParams();
    actorBSpriteParams.scale = size;
    actorBSpriteParams.scaleAxis = Axis::Width;
    actorBSpriteParams.xAnchor = Anchor::LeftIn;
    actorBSpriteParams.yAnchor = Anchor::Center;
    actorBSpriteParams.xPadding = GetPartialElementSizeOnAxis("background", Axis::Width, size); // Should not access to root element with its key ?
    m_root->BuildChild(std::move(actorBSprite));

    UiParams& mainBoxParams = mainBox->GetParams();
    mainBoxParams.scale = 0.5f;
    mainBoxParams.scaleAxis = Axis::Width;
    mainBoxParams.xAnchor = Anchor::Center;
    mainBoxParams.yAnchor = Anchor::BottomIn;
    mainBoxParams.yPadding = GetPartialElementSizeOnAxis("background", Axis::Height, -0.05f); // Should not access to root element with its key ?
    m_root->BuildChild(std::move(mainBox));

    UiParams& actorABoxParams = actorABox->GetParams();
    actorABoxParams.scale = 1.f;
    actorABoxParams.scaleAxis = Axis::Width;
    actorABoxParams.xAnchor = Anchor::RightIn;
    actorABoxParams.yAnchor = Anchor::BottomIn;
    actorABoxParams.xPadding = GetPartialElementSizeOnAxis("actorASprite", Axis::Width, size);
    m_elements["actorASprite"]->BuildChild(std::move(actorABox));

    UiParams& actorBBoxParams = actorBBox->GetParams();
    actorBBoxParams.scale = 1.f;
    actorBBoxParams.scaleAxis = Axis::Width;
    actorBBoxParams.xAnchor = Anchor::RightIn;
    actorBBoxParams.yAnchor = Anchor::BottomIn;
    actorBBoxParams.xPadding = GetPartialElementSizeOnAxis("actorBSprite", Axis::Width, size);
    m_elements["actorBSprite"]->BuildChild(std::move(actorBBox));
    
    UiParams& actorANameParams = actorAName->GetParams();
    actorANameParams.scale = 1.f;
    actorANameParams.xAnchor = Anchor::LeftIn;
    actorANameParams.yAnchor = Anchor::TopIn;
    actorANameParams.xPadding = GetPartialElementSizeOnAxis("actorABox", Axis::Height, 0.15f);
    actorANameParams.yPadding = GetPartialElementSizeOnAxis("actorABox", Axis::Height, 0.15f);
    m_elements["actorABox"]->BuildChild(std::move(actorAName));
    
    UiParams& actorAHealthParams = actorAHealth->GetParams();
    actorAHealthParams.scale = 1.f;
    actorAHealthParams.xAnchor = Anchor::Center;
    actorAHealthParams.yAnchor = Anchor::BottomOut;
    actorAHealthParams.xPadding = GetPartialElementSizeOnAxis("actorAName", Axis::Width, 0.05f);
    m_elements["actorAName"]->BuildChild(std::move(actorAHealth)); // Could also be child of actorABox

    UiParams& actorBNameParams = actorBName->GetParams();
    actorBNameParams.scale = 1.f;
    actorBNameParams.xAnchor = Anchor::LeftIn;
    actorBNameParams.yAnchor = Anchor::TopIn;
    actorBNameParams.xPadding = GetPartialElementSizeOnAxis("actorBBox", Axis::Height, 0.15f);
    actorBNameParams.yPadding = GetPartialElementSizeOnAxis("actorBBox", Axis::Height, 0.15f);
    m_elements["actorBBox"]->BuildChild(std::move(actorBName));

    UiParams& actorBHealthParams = actorBHealth->GetParams();
    actorBHealthParams.scale = 1.f;
    actorBHealthParams.xAnchor = Anchor::Center;
    actorBHealthParams.yAnchor = Anchor::BottomOut;
    actorBHealthParams.xPadding = GetPartialElementSizeOnAxis("actorBName", Axis::Width, 0.05f);
    m_elements["actorBName"]->BuildChild(std::move(actorBHealth)); // Could also be child of actorBBox

    m_root->UpdatePosition(); // Call UpdatePosition on the root UiElement
}

void BattleUiController::Update()
{

}
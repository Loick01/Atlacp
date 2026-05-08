#include "ui_controller.hpp"

UiController::UiController(const AreaSize size, const ScreenPosition position):
    m_size(size), m_position(position)
{}

void UiController::Draw() const
{
    for (const std::unique_ptr<UiElement>& e : m_subRoots)
        e->DrawTexture();
}

void UiController::UpdatePosition()
{
    for (const std::unique_ptr<UiElement>& e : m_subRoots)
        e->UpdatePosition();
}

void UiController::AddElement(const ElementKey& key, UiElement* element)
{
    m_elements[key] = element;
}

void UiController::RemoveElement(const ElementKey& key)
{
    std::unordered_map<ElementKey, UiElement*>::const_iterator it = m_elements.find(key); // Should use a GetIteratorOnElement function ?
    if (it == m_elements.end())
        throw std::runtime_error("This Ui element can't be found : " + key); // ???
    m_elements.erase(it);
}

void UiController::DeleteElement(const ElementKey& key)
{
    UiElement* parent = GetElement(key)->GetParent(); // Should be const ?
    
    // Remove from UiController::m_subRoots or UiElement::m_childs
    std::unique_ptr<UiElement> elementPtr; // This is what I want to get
    
    // TODO Merge
    if (parent == nullptr) { // if UiElement with key is a subroot, its parent is nullptr (as setted in UiController::BuildSubRoot())
        std::vector<std::unique_ptr<UiElement>>::iterator it;
        for (it = m_subRoots.begin() ; it != m_subRoots.end() ; it++) {
            if ((*it)->GetKey() == key) {
                elementPtr = std::move(*it); // Or directly delete the pointer here ?
                m_subRoots.erase(it);
                break;
            }
        }
    } else {
        std::vector<std::unique_ptr<UiElement>>& childs = parent->GetChilds();
        std::vector<std::unique_ptr<UiElement>>::iterator it;
        for (it = childs.begin() ; it != childs.end() ; it++) {
            if ((*it)->GetKey() == key) {
                elementPtr = std::move(*it); // Or directly delete the pointer here ?
                childs.erase(it);
                break;
            }
        }
    }
    
    // Free the unique_ptr
    elementPtr.reset(); // UiElement destructor will call Notify(Delete) --> RemoveElement(key) on himself and all its children 
}

void UiController::BuildSubRoot(std::unique_ptr<UiElement> subRoot)
{
    subRoot->SetParent(nullptr); // Already initialized to nullptr in UiElement constructors
    subRoot->SetParentSize(m_size);
    subRoot->SetParentPosition(m_position);
    const UiParams& params = subRoot->GetParams();
    subRoot->ComputeZoom(params.scale, params.scaleAxis);
    subRoot->ComputePosition(params.xAnchor, params.yAnchor);
    subRoot->SetPadding(params.xPadding, params.yPadding);
    m_subRoots.push_back(std::move(subRoot)); 
}

UiElement* UiController::GetElement(const ElementKey& key) const
{
    std::unordered_map<ElementKey, UiElement*>::const_iterator it = m_elements.find(key);
    if (it == m_elements.end())
        throw std::runtime_error("This Ui element can't be found : " + key);
    // Verify if != nullptr ?
    return it->second;
}

float UiController::GetPartialElementSizeOnAxis(const ElementKey& key, const Axis axis, const float amount) const
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

float UiController::GetPartialRootSizeOnAxis(const Axis axis, const float amount) const
{
    switch(axis) {
        case Axis::Width : {
            return m_size.x * amount;
            break;
        }
        case Axis::Height : {
            return m_size.y * amount;
            break;
        }
        default : {
            throw std::invalid_argument("Unknown axis value\n");
        }
    }
}

std::unique_ptr<UiElement> UiController::CreateElement(TextureController& textureController, 
    const ElementKey& key, const std::string& textureFilepath)
{
    std::unique_ptr<UiElement> element = std::make_unique<UiElement>(textureController, key, textureFilepath);
    element->AddCallback([this, key](UiElementEvent e){HandleUiEvent(e, key);});
    AddElement(key, element.get()); // This function could be called as a Callback if the UiElement was instantiated before its creation
    return element;
}

std::unique_ptr<TextArea> UiController::CreateTextElement(TextureController& textureController, 
    const ElementKey& key, const std::string& fontFilepath)
{
    std::unique_ptr<TextArea> element = std::make_unique<TextArea>(textureController, key, fontFilepath);
    element->AddCallback([this, key](UiElementEvent e){HandleUiEvent(e, key);});
    AddElement(key, element.get()); // This function could be called as a Callback if the UiElement was instantiated before its creation
    return element;
}

void UiController::HandleUiEvent(const UiElementEvent e, const ElementKey& key)
{
    switch(e) {
        case UiElementEvent::Delete : {
            RemoveElement(key);
            break;
        }
        default:
            break;
    }
}

void UiController::UpdateText(const ElementKey& key, const std::string& newText) // Could be in TextArea ?
{
    TextArea* textArea = static_cast<TextArea*>(GetElement(key)); // ???
    textArea->DeleteTexture(); // Delete the previous generated texture
    textArea->SetText(newText);
    textArea->GenerateText();
    // textArea->ComputeFinal(); // I would need anchor, scale, padding, etc.
    // textArea->UpdatePosition();
}

GameplayUiController::GameplayUiController(TextureController& textureController, const std::string& fontFilepath,
    const AreaSize viewportSize, const ScreenPosition viewportPosition):
    UiController(viewportSize, viewportPosition)
{
    std::unique_ptr<UiElement> frame = CreateElement(textureController, "frame", "../assets/ui/box.png");
    std::unique_ptr<UiElement> faceset = CreateElement(textureController, "faceset", "../assets/ui/faceset.png");
    std::unique_ptr<UiElement> face = CreateElement(textureController, "face", "../assets/ui/hunter_face.png");
    std::unique_ptr<TextArea> boxText = CreateTextElement(textureController, "boxText", fontFilepath);

    UiParams& frameParams = frame->GetParams();
    frameParams.scale = GetPartialRootSizeOnAxis(Axis::Width, 0.5f);
    frameParams.scaleAxis = Axis::Width;
    frameParams.xAnchor = Anchor::Center;
    frameParams.yAnchor = Anchor::BottomIn;
    frameParams.yPadding = GetPartialRootSizeOnAxis(Axis::Height, -0.05f);
    BuildSubRoot(std::move(frame));

    UiParams& facesetParams = faceset->GetParams();
    facesetParams.scale = GetPartialElementSizeOnAxis("frame", Axis::Height, 0.7f);
    facesetParams.scaleAxis = Axis::Height;
    facesetParams.xAnchor = Anchor::LeftIn;
    facesetParams.yAnchor = Anchor::Center;
    facesetParams.xPadding = GetPartialElementSizeOnAxis("frame", Axis::Height, 0.15f); // Should not access to root element with its key ?
    GetElement("frame")->BuildChild(std::move(faceset));

    boxText->SetText("Hello world ! This is an example of a long sentence to test how the text is wrapped by SDL_ttf...");
    UiParams& boxTextParams = boxText->GetParams();
    boxTextParams.scale = GetPartialElementSizeOnAxis("frame", Axis::Width, 0.75f);
    boxTextParams.xAnchor = Anchor::RightOut;
    boxTextParams.yAnchor = Anchor::Center;
    boxTextParams.xPadding = GetPartialElementSizeOnAxis("faceset", Axis::Width, 0.2f);
    GetElement("faceset")->BuildChild(std::move(boxText));

    UiParams& faceParams = face->GetParams();
    faceParams.scale = GetPartialElementSizeOnAxis("faceset", Axis::Width, 0.8f);
    faceParams.scaleAxis = Axis::Width;
    faceParams.xAnchor = Anchor::Center;
    faceParams.yAnchor = Anchor::Center;
    GetElement("faceset")->BuildChild(std::move(face));

    UpdatePosition(); // Call UpdatePosition on all the branches
}

void GameplayUiController::Update()
{}

EditorUiController::EditorUiController(TextureController& textureController, const std::string& fontFilepath,
    const AreaSize viewportSize, const ScreenPosition viewportPosition):
    UiController(viewportSize, viewportPosition), m_lastLayer(0) // lastLayer should be initialized with EditorEventState::selectedLayer ?
{
    std::unique_ptr<UiElement> frame = CreateElement(textureController, "frame", "../assets/ui/box.png");
    std::unique_ptr<TextArea> boxText = CreateTextElement(textureController, "boxText", fontFilepath);

    UiParams& frameParams = frame->GetParams();
    frameParams.scale = GetPartialRootSizeOnAxis(Axis::Width, 0.3f);
    frameParams.scaleAxis = Axis::Width;
    frameParams.xAnchor = Anchor::LeftIn;
    frameParams.yAnchor = Anchor::TopIn;
    frameParams.yPadding = GetPartialRootSizeOnAxis(Axis::Width, 0.02f);
    BuildSubRoot(std::move(frame));

    boxText->SetText("Selected layer : " + std::to_string(m_lastLayer));
    UiParams& boxTextParams = boxText->GetParams();
    boxTextParams.scale = GetPartialElementSizeOnAxis("frame", Axis::Width, 0.9f);
    boxTextParams.xAnchor = Anchor::LeftIn;
    boxTextParams.yAnchor = Anchor::Center;
    boxTextParams.xPadding = GetPartialElementSizeOnAxis("frame", Axis::Width, 0.1f); // Should not access to root element with its key ?
    GetElement("frame")->BuildChild(std::move(boxText));

    UpdatePosition(); // Call UpdatePosition on all the branches
}

void EditorUiController::Update()
{
    if (m_eventState.selectedLayer != m_lastLayer){
        m_lastLayer = m_eventState.selectedLayer;
        UpdateText("boxText", "Selected layer : " + std::to_string(m_lastLayer));
    } 
}

BattleUiController::BattleUiController(TextureController& textureController, const std::string& fontFilepath,
    const AreaSize viewportSize, const ScreenPosition viewportPosition):
    UiController(viewportSize, viewportPosition)
{
    std::unique_ptr<UiElement> background = CreateElement(textureController, "background", "../assets/battle/backgrounds/cavern.png");
    std::unique_ptr<UiElement> actorASprite = CreateElement(textureController, "actorASprite", "../assets/battle/werewolf.png");
    std::unique_ptr<UiElement> actorBSprite = CreateElement(textureController, "actorBSprite", "../assets/battle/bone_appetit.png");
    std::unique_ptr<UiElement> actorABox = CreateElement(textureController, "actorABox", "../assets/ui/box.png");
    std::unique_ptr<UiElement> actorBBox = CreateElement(textureController, "actorBBox", "../assets/ui/box.png");
    std::unique_ptr<UiElement> mainBox = CreateElement(textureController, "mainBox", "../assets/ui/box.png");
    std::unique_ptr<TextArea> actorAName = CreateTextElement(textureController, "actorAName", fontFilepath);
    std::unique_ptr<TextArea> actorBName = CreateTextElement(textureController, "actorBName", fontFilepath);
    std::unique_ptr<TextArea> actorAHealth = CreateTextElement(textureController, "actorAHealth", fontFilepath);
    std::unique_ptr<TextArea> actorBHealth = CreateTextElement(textureController, "actorBHealth", fontFilepath);

    const float size = 0.2f; // Will be removed
    UiParams& backgroundParams = background->GetParams();
    backgroundParams.scale = GetPartialRootSizeOnAxis(Axis::Height, 1.f);
    backgroundParams.scaleAxis = Axis::Height;
    backgroundParams.xAnchor = Anchor::LeftIn;
    backgroundParams.yAnchor = Anchor::TopIn;
    BuildSubRoot(std::move(background));

    UiParams& actorASpriteParams = actorASprite->GetParams();
    actorASpriteParams.scale = GetPartialElementSizeOnAxis("background", Axis::Width, size);
    actorASpriteParams.scaleAxis = Axis::Width;
    actorASpriteParams.xAnchor = Anchor::RightIn;
    actorASpriteParams.yAnchor = Anchor::Center;
    actorASpriteParams.xPadding = GetPartialElementSizeOnAxis("background", Axis::Width, -size); // Should not access to root element with its key ?
    GetElement("background")->BuildChild(std::move(actorASprite));

    UiParams& actorBSpriteParams = actorBSprite->GetParams();
    actorBSpriteParams.scale = GetPartialElementSizeOnAxis("background", Axis::Width, size);;
    actorBSpriteParams.scaleAxis = Axis::Width;
    actorBSpriteParams.xAnchor = Anchor::LeftIn;
    actorBSpriteParams.yAnchor = Anchor::Center;
    actorBSpriteParams.xPadding = GetPartialElementSizeOnAxis("background", Axis::Width, size); // Should not access to root element with its key ?
    GetElement("background")->BuildChild(std::move(actorBSprite));

    UiParams& mainBoxParams = mainBox->GetParams();
    mainBoxParams.scale = GetPartialElementSizeOnAxis("background", Axis::Width, 0.5f);;
    mainBoxParams.scaleAxis = Axis::Width;
    mainBoxParams.xAnchor = Anchor::Center;
    mainBoxParams.yAnchor = Anchor::BottomIn;
    mainBoxParams.yPadding = GetPartialElementSizeOnAxis("background", Axis::Height, -0.05f); // Should not access to root element with its key ?
    GetElement("background")->BuildChild(std::move(mainBox));

    UiParams& actorABoxParams = actorABox->GetParams();
    actorABoxParams.scale = GetPartialElementSizeOnAxis("actorASprite", Axis::Width, 1.f);
    actorABoxParams.scaleAxis = Axis::Width;
    actorABoxParams.xAnchor = Anchor::RightIn;
    actorABoxParams.yAnchor = Anchor::BottomIn;
    actorABoxParams.xPadding = GetPartialElementSizeOnAxis("actorASprite", Axis::Width, size);
    GetElement("actorASprite")->BuildChild(std::move(actorABox));

    UiParams& actorBBoxParams = actorBBox->GetParams();
    actorBBoxParams.scale = GetPartialElementSizeOnAxis("actorBSprite", Axis::Width, 1.f);
    actorBBoxParams.scaleAxis = Axis::Width;
    actorBBoxParams.xAnchor = Anchor::RightIn;
    actorBBoxParams.yAnchor = Anchor::BottomIn;
    actorBBoxParams.xPadding = GetPartialElementSizeOnAxis("actorBSprite", Axis::Width, size);
    GetElement("actorBSprite")->BuildChild(std::move(actorBBox));
    
    UiParams& actorANameParams = actorAName->GetParams();
    actorANameParams.scale = GetPartialElementSizeOnAxis("actorABox", Axis::Width, 1.f);
    actorANameParams.xAnchor = Anchor::LeftIn;
    actorANameParams.yAnchor = Anchor::TopIn;
    actorANameParams.xPadding = GetPartialElementSizeOnAxis("actorABox", Axis::Height, 0.15f);
    actorANameParams.yPadding = GetPartialElementSizeOnAxis("actorABox", Axis::Height, 0.15f);
    GetElement("actorABox")->BuildChild(std::move(actorAName));
    
    UiParams& actorAHealthParams = actorAHealth->GetParams();
    actorAHealthParams.scale = GetPartialElementSizeOnAxis("actorAName", Axis::Width, 1.f);
    actorAHealthParams.xAnchor = Anchor::Center;
    actorAHealthParams.yAnchor = Anchor::BottomOut;
    GetElement("actorAName")->BuildChild(std::move(actorAHealth)); // Could also be child of actorABox

    UiParams& actorBNameParams = actorBName->GetParams();
    actorBNameParams.scale = GetPartialElementSizeOnAxis("actorBBox", Axis::Width, 1.f);
    actorBNameParams.xAnchor = Anchor::LeftIn;
    actorBNameParams.yAnchor = Anchor::TopIn;
    actorBNameParams.xPadding = GetPartialElementSizeOnAxis("actorBBox", Axis::Height, 0.15f);
    actorBNameParams.yPadding = GetPartialElementSizeOnAxis("actorBBox", Axis::Height, 0.15f);
    GetElement("actorBBox")->BuildChild(std::move(actorBName));

    UiParams& actorBHealthParams = actorBHealth->GetParams();
    actorBHealthParams.scale = GetPartialElementSizeOnAxis("actorBName", Axis::Width, 1.f);
    actorBHealthParams.xAnchor = Anchor::Center;
    actorBHealthParams.yAnchor = Anchor::BottomOut;
    GetElement("actorBName")->BuildChild(std::move(actorBHealth)); // Could also be child of actorBBox

    UpdatePosition(); // Call UpdatePosition on all the branches
}

void BattleUiController::Update()
{}
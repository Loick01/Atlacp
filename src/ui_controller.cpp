#include "ui_controller.hpp"

UiController::UiController(const FileReader& fileReader, TextureController& textureController, const std::string& fontFilepath,
const AreaSize size, const ScreenPosition position):
    m_fileReader(fileReader), m_textureController(textureController), m_fontFilepath(fontFilepath), m_size(size), m_position(position)
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
    
    if (parent == nullptr) // UiElement key = nullptr is a subroot (as setted in UiController::BuildSubRoot())
        elementPtr = RemoveSubRoots(key);
    else
        elementPtr = parent->RemoveChild(key);
    
    // Free the unique_ptr
    elementPtr.reset(); // UiElement destructor will call Notify(Delete) --> RemoveElement(key) on himself and all its children 
}

void UiController::BuildSubRoot(std::unique_ptr<UiElement> subRoot)
{
    subRoot->SetParent(nullptr); // Already initialized to nullptr in UiElement constructors
    subRoot->SetParentSize(m_size);
    subRoot->SetParentPosition(m_position);
    const UiParams& params = subRoot->GetParams();
    subRoot->ComputeFinal();
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

std::unique_ptr<UiElement> UiController::CreateElement(const ElementKey& key, const std::string& textureFilepath)
{
    std::unique_ptr<UiElement> element = std::make_unique<UiElement>(m_textureController, key, textureFilepath);
    element->AddCallback([this, key](UiElementEvent e){HandleUiEvent(e, key);});
    AddElement(key, element.get()); // This function could be called as a Callback if the UiElement was instantiated before its creation
    return element;
}

std::unique_ptr<TextArea> UiController::CreateTextElement(const ElementKey& key, const std::string& fontFilepath)
{
    std::unique_ptr<TextArea> element = std::make_unique<TextArea>(m_textureController, key, fontFilepath);
    element->AddCallback([this, key](UiElementEvent e){HandleUiEvent(e, key);});
    AddElement(key, element.get()); // This function could be called as a Callback if the UiElement was instantiated before its creation
    return element;
}

std::unique_ptr<TextArea> UiController::CreateTextElement(const ElementKey& key)
{
    return CreateTextElement(key, m_fontFilepath);
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

void UiController::UpdateText(const ElementKey& key, const std::string& text)
{
    TextArea* textArea = static_cast<TextArea*>(GetElement(key)); // static_cast ?
    textArea->DeleteTexture(); // Delete the previous generated texture
    textArea->SetText(text);
    textArea->ComputeFinal(); // Load the new texture (GenerateText())
    // textArea->UpdatePosition(); // ?
}

std::unique_ptr<UiElement> UiController::RemoveSubRoots(const ElementKey& key) // Same than UiElement::RemoveChild
{
    std::vector<std::unique_ptr<UiElement>>::iterator it;
    for (it = m_subRoots.begin() ; it != m_subRoots.end() ; it++) {
        if ((*it)->GetKey() == key) {
            std::unique_ptr<UiElement> removed = std::move(*it);
            m_subRoots.erase(it);
            return removed;
        }
    }
    throw std::runtime_error("No subroots have this key : " + key);
}


void UiController::UpdateParent(const ElementKey& key, const ElementKey& parent)
{
    // Could use UiElement::BuildChild() ?
    UiElement* currentElement = GetElement(key);
    UiElement* previousParent = currentElement->GetParent();
    UiElement* newParent = GetElement(parent);
    currentElement->SetParent(newParent);
    currentElement->SetParentSize(newParent->GetSize());
    currentElement->ComputeFinal(); // WARNING : currentElement has the same UiParams as with the previous parent, which may be not wanted
    std::unique_ptr<UiElement> e;
    if (previousParent == nullptr) { // currentElement is a subRoot
        e = RemoveSubRoots(key);
    } else {
        e = previousParent->RemoveChild(key);
    }
    newParent->AddChild(e);
}

void UiController::BuildUiFile(const std::string& filepath) 
{
    std::vector<DataUi> fileResult = m_fileReader.ReadUiFile(filepath);

    for (const DataUi& data : fileResult) { // Will not be here
        std::unique_ptr<UiElement> element;
        if (data.type == "uielement") {
            element = CreateElement(data.key, data.path);
        } else if (data.type == "textarea") {
            std::unique_ptr<TextArea> textAreaElement = CreateTextElement(data.key, data.path);
            textAreaElement->SetText(data.text); // SetText can only be called on a TextArea, not UiElement
            element = std::move(textAreaElement);
        } else {
            throw std::runtime_error("Unknown element type : " + data.type); // ?
        }

        UiParams& params = element->GetParams();
        
        if (data.scale.srcElement == "root")
            params.scale = GetPartialRootSizeOnAxis(data.scale.axis, data.scale.amount);
        else 
            params.scale = GetPartialElementSizeOnAxis(data.scale.srcElement, data.scale.axis, data.scale.amount);
        
        params.scaleAxis = data.dstScaleAxis; // Also when TextArea ?
        params.xAnchor = data.xAnchor;
        params.yAnchor = data.yAnchor;
        
        const PartialSize xPaddingData = data.xPadding;
        const PartialSize yPaddingData = data.yPadding;
        if (xPaddingData.srcElement != "undefined_element") {
            if (xPaddingData.srcElement == "root")
                params.xPadding = GetPartialRootSizeOnAxis(xPaddingData.axis, xPaddingData.amount);
            else 
                params.xPadding = GetPartialElementSizeOnAxis(xPaddingData.srcElement, xPaddingData.axis, xPaddingData.amount);
        }

        if (yPaddingData.srcElement != "undefined_element")  {
            if (yPaddingData.srcElement == "root")
                params.yPadding = GetPartialRootSizeOnAxis(yPaddingData.axis, yPaddingData.amount);
            else 
                params.yPadding = GetPartialElementSizeOnAxis(yPaddingData.srcElement, yPaddingData.axis, yPaddingData.amount);
        }
        
        if (data.parentKey == "root") {
            BuildSubRoot(std::move(element));
        } else {
            GetElement(data.parentKey)->BuildChild(std::move(element));
        }
    }
}

void UiController::OpenDialogBox(const std::string& text)
{
    BuildUiFile("../data/ui/dialog_box_template");
    UpdateText("boxText", text);
    GetElement("frame")->UpdatePosition(); // Or UiController::UpdatePosition() ?
}

GameplayUiController::GameplayUiController(const FileReader& fileReader, TextureController& textureController, const std::string& fontFilepath,
    const AreaSize viewportSize, const ScreenPosition viewportPosition):
    UiController(fileReader, textureController, fontFilepath, viewportSize, viewportPosition)
{}

void GameplayUiController::Update()
{}

EditorUiController::EditorUiController(const FileReader& fileReader, TextureController& textureController, const std::string& fontFilepath,
    const AreaSize viewportSize, const ScreenPosition viewportPosition):
    UiController(fileReader, textureController, fontFilepath, viewportSize, viewportPosition), m_lastLayer(-1) // lastLayer should be initialized with EditorEventState::selectedLayer ?
{
    
    BuildUiFile("../data/ui/editor_scene");
    UpdatePosition();
}

void EditorUiController::Update()
{
    if (m_eventState.selectedLayer != m_lastLayer){
        m_lastLayer = m_eventState.selectedLayer;
        UpdateText("boxText", "Selected layer : " + std::to_string(m_lastLayer));
    } 
}

BattleUiController::BattleUiController(const FileReader& fileReader, TextureController& textureController, const std::string& fontFilepath,
    const AreaSize viewportSize, const ScreenPosition viewportPosition):
    UiController(fileReader, textureController, fontFilepath, viewportSize, viewportPosition)
{
    BuildUiFile("../data/ui/battle_scene");
    UpdatePosition();
}

void BattleUiController::Update()
{}
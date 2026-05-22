#include "ui_controller.hpp"

UiController::UiController(const FileReader& fileReader, TextureController& textureController, const std::string& fontFilepath):
    m_fileReader(fileReader), m_textureController(textureController), m_fontFilepath(fontFilepath)
{} // WARNING : m_size and m_position are not defined, must use SetSize()/SetPosition()

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

std::unordered_map<ElementKey, UiElement*>::const_iterator UiController::GetIteratorOnElement(const ElementKey& key) const
{
    return m_elements.find(key);
}

void UiController::RemoveElement(const ElementKey& key)
{
    std::unordered_map<ElementKey, UiElement*>::const_iterator it = GetIteratorOnElement(key);
    if (it == m_elements.end())
        throw std::runtime_error("(UiController::RemoveElement) This Ui element can't be found : " + key); // ???
    m_elements.erase(it);
}

void UiController::Clear()
{
    while (!m_subRoots.empty()) // Do not use for loop (because m_subRoots is modified)
        DeleteElement(m_subRoots[0]->GetKey());
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

void UiController::SetSize(const AreaSize size)
{
    m_size = size;
}
        
void UiController::SetPosition(const ScreenPosition position)
{
    m_position = position;
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
    std::unordered_map<ElementKey, UiElement*>::const_iterator it = GetIteratorOnElement(key);
    if (it == m_elements.end())
        throw std::runtime_error("(UiController::GetElement) This Ui element can't be found : " + key);
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
    textArea->UpdatePosition();
}

void UiController::UpdateText(const UiValue<std::string>& uiv)
{
    UpdateText(uiv.id, uiv.value);
}

void UiController::UpdateText(const UiValue<unsigned int>& uiv)
{
    UpdateText(uiv.id, std::to_string(uiv.value));
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
    throw std::runtime_error("(UiController::RemoveSubRoots) No subroots have this key : " + key);
}


void UiController::UpdateParent(const ElementKey& key, const ElementKey& parent)
{
    UiElement* currentElement = GetElement(key);
    UiElement* previousParent = currentElement->GetParent();
    UiElement* newParent = GetElement(parent);
    
    std::unique_ptr<UiElement> ownedElement; // unique_ptr of currentElement
    if (previousParent == nullptr) // subRoot has no parent
        ownedElement = RemoveSubRoots(key);
    else
        ownedElement = previousParent->RemoveChild(key);

     // WARNING : currentElement has the same UiParams as with 
     // the previous parent, which may be not wanted
    newParent->BuildChild(std::move(ownedElement));
}

void UiController::BuildUiFile(const std::string& filepath) 
{
    std::vector<DataUi> fileResult = m_fileReader.ReadUiFile(filepath);

    for (const DataUi& data : fileResult) { // Will not be here

        if (GetIteratorOnElement(data.key) != m_elements.end()) continue; // I assumed it is possible to try to create elements that already exist   

        std::unique_ptr<UiElement> element;
        if (data.type == "uielement") {
            element = CreateElement(data.key, data.path);
        } else if (data.type == "textarea") {
            std::unique_ptr<TextArea> textAreaElement = CreateTextElement(data.key, data.path);
            textAreaElement->SetText(data.text);
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
    UpdatePosition(); // Maybe I could only called UpdatePosition only on the subroots created in this call ? 
}

void UiController::OpenDialogBox(const std::string& text)
{
    BuildUiFile("../data/ui/dialog_box_template");
    UpdateText("dialogText", text);
}
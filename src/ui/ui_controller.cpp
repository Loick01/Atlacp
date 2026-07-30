#include "ui/ui_controller.hpp"

#include <stdexcept>

#include "core/file.hpp"
#include "image/texture.hpp"

UiController::UiController(const FileReader& fileReader, TextureController& textureController, const std::string& fontFilepath):
    m_fileReader(fileReader), m_textureController(textureController), m_fontFilepath(fontFilepath)
{} // WARNING : m_size and m_position are not defined, must use SetSize()/SetPosition()

std::string UiController::GetFileExtension(const std::string& filepath) const // Will be in FileReader
{
    size_t pos = filepath.rfind('.');
    if (pos == std::string::npos)
        throw std::runtime_error("This UI file has no extension : " + filepath);
    return filepath.substr(pos + 1);
}

void UiController::AddElement(const UiKey& key, UiElement* element)
{
    m_elements[key] = element;
}

void UiController::RemoveElement(const UiKey& key)
{
    std::unordered_map<UiKey, UiElement*>::const_iterator it = GetIteratorOnElement(key);
    if (it == m_elements.end())
        throw std::runtime_error("(UiController::RemoveElement) This Ui element can't be found : " + key); // This should not happen. Remove ?
    m_elements.erase(it);
}

void UiController::HandleUiEvent(const UiElementEvent e, const UiKey& key)
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

void UiController::UpdatePosition()
{
    for (const std::unique_ptr<UiElement>& e : m_subRoots)
        e->UpdatePosition();
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


std::unique_ptr<UiElement> UiController::CreateElement(const UiKey& key, const std::string& textureFilepath)
{
    return std::make_unique<UiElement>(m_textureController, key, textureFilepath);
}

std::unique_ptr<UiTextElement> UiController::CreateTextElement(const UiKey& key)
{
    return std::make_unique<UiTextElement>(m_textureController, key);
}

std::unique_ptr<UiAnimatedElement> UiController::CreateAnimatedElement(const UiKey& key, const std::string& animationPath)
{
    return std::make_unique<UiAnimatedElement>(m_fileReader, m_textureController, key, animationPath);
}

std::unique_ptr<UiElement> UiController::RemoveSubRoots(const UiKey& key) // Same than UiElement::RemoveChild
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

std::unique_ptr<UiElement> UiController::GenerateElementFromData(const DataUi& data)
{
    std::unique_ptr<UiElement> element;
    if (data.type == "uielement") {
        element = CreateElement(data.key, data.imagePath);
    } else if (data.type == "textelement") {
        std::unique_ptr<UiTextElement> textElement = CreateTextElement(data.key);
        textElement->SetText(data.text);
        textElement->SetTextSize(data.fontSize);
        element = std::move(textElement);
    } else if (data.type == "animatedelement") {
        element = CreateAnimatedElement(data.key, data.imagePath);
    } else {
        throw std::runtime_error("Unknown UI element type : " + data.type);
    }

    UiParams& params = element->GetParams();
    
    params.scale = GetResultFromPartialSize(data.scale);
    
    params.scaleAxis = data.dstScaleAxis; // Also when UiTextElement ?
    params.xAnchor = data.xAnchor;
    params.yAnchor = data.yAnchor;
    
    const PartialSize xPaddingData = data.xPadding;
    const PartialSize yPaddingData = data.yPadding;
    if (xPaddingData.srcElement != "undefined_element")
        params.xPadding = GetResultFromPartialSize(xPaddingData);

    if (yPaddingData.srcElement != "undefined_element")
        params.yPadding = GetResultFromPartialSize(yPaddingData);

    return element;
}

std::vector<UiKey> UiController::BuildUiFile(const std::string& filepath) 
{
    std::vector<DataUi> fileResult = m_fileReader.ReadUiFile(filepath);
    std::vector<UiKey> createdElements;
    
    for (const DataUi& data : fileResult) { 
        const UiKey& k = data.key;
        if (GetIteratorOnElement(k) != m_elements.end()) continue; // I assumed it is possible to try to create elements that already exist   
        createdElements.push_back(k); // Maybe I could use a flag in ui file to mark specific elements
        std::unique_ptr<UiElement> element = GenerateElementFromData(data);
        element->AddCallback([this, k](UiElementEvent e){HandleUiEvent(e, k);});
        BuildElement(element, data.parentKey);
    }
    UpdatePosition(); // Maybe I could only called UpdatePosition only on the subroots created in this call ? 
    return createdElements;
}

std::unordered_map<UiKey, UiElement*>::const_iterator UiController::GetIteratorOnElement(const UiKey& key) const
{
    return m_elements.find(key);
}

UiElement* UiController::GetElement(const UiKey& key) const
{
    std::unordered_map<UiKey, UiElement*>::const_iterator it = GetIteratorOnElement(key);
    if (it == m_elements.end())
        throw std::runtime_error("(UiController::GetElement) This Ui element can't be found : " + key);
    return it->second;
}

float UiController::GetPartialElementSizeOnAxis(const UiKey& key, const Axis axis, const float amount) const
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

float UiController::GetResultFromPartialSize(const PartialSize& ps) const
{
    if (ps.srcElement == "root")
        return GetPartialRootSizeOnAxis(ps.axis, ps.amount);
    else 
        return GetPartialElementSizeOnAxis(ps.srcElement, ps.axis, ps.amount);
}

bool UiController::IsBaseUiFile(const std::string& filepath) const
{
    return GetFileExtension(filepath) == "uif";
}

bool UiController::IsTemplateUiFile(const std::string& filepath) const
{
    return GetFileExtension(filepath) == "uit";
}

void UiController::ClearAll()
{
    while (!m_subRoots.empty()) // Do not use for loop (because m_subRoots is modified)
        DeleteElement(m_subRoots[0]->GetKey());
}

void UiController::BuildElement(std::unique_ptr<UiElement>& element, const UiKey& parentKey)
{
    AddElement(element->GetKey(), element.get());
    if (parentKey == "root") {
        BuildSubRoot(std::move(element));
    } else {
        GetElement(parentKey)->BuildChild(std::move(element));
    }
}

void UiController::DeleteElement(const UiKey& key)
{
    UiElement* const parent = GetElement(key)->GetParent();
    
    std::unique_ptr<UiElement> ownedElement; 
    // Remove from UiController::m_subRoots or UiElement::m_childs and retrieve the unqiue_ptr
    if (parent == nullptr) // UiElement key = nullptr is a subroot (as setted in UiController::BuildSubRoot())
        ownedElement = RemoveSubRoots(key);
    else
        ownedElement = parent->RemoveChild(key);

    // Free the unique_ptr
    ownedElement.reset(); // UiElement destructor will call Notify(Delete) --> RemoveElement(key) on himself and all its children 
}

void UiController::SetSize(const AreaSize size)
{
    m_size = size;
}
        
void UiController::SetPosition(const ScreenPosition position)
{
    m_position = position;
}

void UiController::UpdatePath(const UiKey& key, const std::string& path)
{
    UiElement* element = GetElement(key);
    element->DeleteTexture(); // Delete the previous used texture in TextureController (and remove its key in TextureController::m_textures)
    element->LoadTexture(path);
    element->ComputeFinal();
    element->UpdatePosition();
}

void UiController::UpdatePath(const UiValue<std::string>& path) // Should I use UpdatePath(const UiKey& key, const std::string& path) ? (same with the different version of UpdateText())
{
    UpdatePath(path.id, path.value);
}

void UiController::UpdateText(const UiKey& key, const std::string& text)
{
    UiTextElement* textElement = static_cast<UiTextElement*>(GetElement(key)); // I assume key will give a UiTextElement key
    textElement->DeleteTexture(); // Delete the previous generated texture
    textElement->SetText(text);
    textElement->ComputeFinal(); // Load the new texture (GenerateText())
    textElement->UpdatePosition();
}

void UiController::UpdateText(const UiValue<std::string>& uiv)
{
    UpdateText(uiv.id, uiv.value);
}

void UiController::UpdateText(const UiValue<unsigned int>& uiv)
{
    UpdateText(uiv.id, std::to_string(uiv.value));
}

void UiController::UpdateParent(const UiKey& key, const UiKey& parent)
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

void UiController::UpdateScalingSize(const UiKey& key, const PartialSize ps)
{
    const float result = GetResultFromPartialSize(ps);
    UiElement* e = GetElement(key);
    e->SetParamsScale(result);
    e->ComputeFinal();
    // Should also update scale size for every children of e ?
    e->UpdatePosition();
}

void UiController::UpdatePadding(const UiKey& key, const PartialSize ps, const Axis axis)
{
    const float result = GetResultFromPartialSize(ps);
    UiElement* e = GetElement(key);
    e->SetParamsPadding(axis, result);
    e->ComputeFinal();
    e->UpdatePosition();
}

void UiController::UpdateKey(const UiKey& key, const UiKey& newKey)
{
    UiElement* element = GetElement(key);
    element->SetKey(newKey);
    element->ClearCallback();
    element->AddCallback([this, newKey](UiElementEvent e){HandleUiEvent(e, newKey);});

    // It's useless to test if key is in m_elements, because GetElement(key) has already done it
    // However, newKey could already be used
    if (m_elements.find(newKey) != m_elements.end()) 
        throw std::runtime_error("Try to update a node in UiController::m_elements with an already existing key : " + newKey);

    // https://stackoverflow.com/questions/5743545/what-is-the-fastest-way-to-change-a-key-of-an-element-inside-stdmap
    std::unordered_map<UiKey, UiElement*>::node_type node = m_elements.extract(key);
    node.key() = newKey;
    m_elements.insert(std::move(node));
}

void UiController::UpdateParams(const UiKey& key, const UiParams& params)
{
    UiElement* element = GetElement(key);
    element->SetParams(params);
    element->ComputeFinal();
    element->UpdatePosition();    
}

void UiController::Draw() const
{
    for (const std::unique_ptr<UiElement>& e : m_subRoots)
        e->DrawTexture();
}

void UiController::OpenDialogBox(const std::string& text) // DialogBox should be a UiComponent ?
{
    BuildUiFile("../data/ui/file/dialog_box.uif");
    UpdateText("dialogText", text);
}
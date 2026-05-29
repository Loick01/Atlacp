#include "ui/ui_element.hpp"

UiElement::UiElement(TextureController& textureController, const ElementKey& key,
    const std::string& textureFilepath, const ScreenPosition localPosition):
    ScreenDrawable(textureController, textureFilepath), m_key(key), m_localPosition(localPosition), m_parent(nullptr)
{}

UiElement::UiElement(TextureController& textureController, const ElementKey& key,
    const ScreenPosition localPosition):
    ScreenDrawable(textureController), m_key(key), m_localPosition(localPosition), m_parent(nullptr)
{}

UiElement::~UiElement() 
{
    Notify(UiElementEvent::Delete); // Will delete the associated pointer in UiController::m_elements
}

std::vector<std::unique_ptr<UiElement>>& UiElement::GetChilds() 
{
    return m_childs;
}

UiParams& UiElement::GetParams()
{
    return m_params;
}

UiElement* UiElement::GetParent()
{
    return m_parent;
}

const ElementKey& UiElement::GetKey() const
{
    return m_key;
}

void UiElement::SetKey(const ElementKey& key)
{
    m_key = key;
}

// void UiElement::SetParent(UiElement const* parent)
void UiElement::SetParent(UiElement* parent)
{
    m_parent = parent;
}

void UiElement::SetParentSize(const AreaSize parentSize)
{
    m_parentSize = parentSize;
}

void UiElement::SetParentPosition(const ScreenPosition parentPosition)
{
    m_parentPosition = parentPosition;
}

void UiElement::SetParams(const UiParams& params)
{
    m_params = params;
}

void UiElement::SetParamsScale(const float scale)
{
    m_params.scale = scale;
}

void UiElement::DrawTexture() const
{
    ScreenDrawable::DrawTexture();
    for (const std::unique_ptr<UiElement>& e : m_childs)
        e->DrawTexture();
}

void UiElement::UpdatePosition()
{
    SetScreenPosition(m_parentPosition + m_localPosition); // ScreenDrawable::m_position is the global position
    for (const std::unique_ptr<UiElement>& e : m_childs) {
        e->SetParentPosition(GetScreenPosition());
        e->UpdatePosition(); // Make sure to update childs position after updating m_position
    }
}

void UiElement::SetLocalPosition(const ScreenPosition localPosition)
{
    m_localPosition = localPosition;
}

void UiElement::AddLocalPosition(const ScreenPosition position)
{
    m_localPosition += position;
}

void UiElement::ComputeZoom(const float scale, const Axis axis)
{
    switch(axis){
        case Axis::Width:
            SetZoom(scale/GetTextureWidth());
            break;
        case Axis::Height:
            SetZoom(scale/GetTextureHeight());
            break;
        default:
            throw std::invalid_argument("Unknown axis value\n");
    }
}

void UiElement::ComputePosition(const Anchor xAnchor, const Anchor yAnchor)
{
    ScreenPosition localPosition = {0,0};
    // If ComputePosition is called on a UiTextElement, the zoom here must be 1.0f (text size is handled by the font), I should use a virtual function to get the zoom here
    const AreaSize size = GetSize(); // Need drawing size (including the zoom) to get the position (so ComputePosition must be called after ComputeZoom) 
    switch(xAnchor){
        case Anchor::LeftIn:
            localPosition.x = 0;
            break;
        case Anchor::LeftOut:
            localPosition.x = -size.x;
            break;
        case Anchor::Center:
            localPosition.x = (m_parentSize.x-size.x)/2;
            break;
        case Anchor::RightIn:
            localPosition.x = m_parentSize.x-size.x;
            break;
        case Anchor::RightOut:
            localPosition.x = m_parentSize.x;
            break;
        default:
            throw std::invalid_argument("Incorrect anchor value for x axis\n");
    }
    switch(yAnchor){
        case Anchor::TopIn:
            localPosition.y = 0;
            break;
        case Anchor::TopOut:
            localPosition.y = -size.y;
            break;
        case Anchor::Center:
            localPosition.y = (m_parentSize.y-size.y)/2;
            break;
        case Anchor::BottomIn:
            localPosition.y = m_parentSize.y-size.y;
            break;
        case Anchor::BottomOut:
            localPosition.y = m_parentSize.y;
            break;
        default:
            throw std::invalid_argument("Incorrect anchor value for y axis\n");
    }
    SetLocalPosition(localPosition);
}

void UiElement::SetPadding(const float xPadding, const float yPadding)
{
    AddLocalPosition({xPadding,yPadding}); // AddLocal ???
}

void UiElement::AddChild(std::unique_ptr<UiElement>& child)
{
    m_childs.push_back(std::move(child));
}

std::unique_ptr<UiElement> UiElement::RemoveChild(const ElementKey& key)
{
    std::vector<std::unique_ptr<UiElement>>::iterator it;
    for (it = m_childs.begin() ; it != m_childs.end() ; it++) {
        if ((*it)->GetKey() == key) {
            std::unique_ptr<UiElement> removed = std::move(*it);
            m_childs.erase(it);
            return removed;
        }
    }
    throw std::runtime_error("(UiElement::RemoveChild) No elements have this key : " + key);
}

void UiElement::BuildChild(std::unique_ptr<UiElement> child)
{
    child->SetParent(this);
    child->SetParentSize(GetSize());
    child->ComputeFinal();
    AddChild(child);
}

void UiElement::ComputeFinal()
{
    ComputeZoom(m_params.scale, m_params.scaleAxis);
    ComputePosition(m_params.xAnchor, m_params.yAnchor);
    SetPadding(m_params.xPadding, m_params.yPadding);
}

UiTextElement::UiTextElement(TextureController& textureController, const ElementKey& key, const std::string& fontFilepath, const SDL_Color color):
    UiElement(textureController, key), m_textColor(color), m_text("No_Text")
{
    const unsigned int fontSize = 24;
    m_fontKey = fontFilepath + std::to_string(fontSize); // fontFilepath is not the full path, just the filename in the font directory
    textureController.LoadFontFromFile("../assets/ui/fonts/"+fontFilepath+".ttf", m_fontKey, fontSize);
}

void UiTextElement::SetText(const std::string& text)
{
    m_text = text;
}

void UiTextElement::GenerateText() // Same than Drawable::LoadTexture(), will be improved
{
    m_textureKey = m_text; // Will use something else than just text as texture key
    m_textureController.LoadTextureFromText(m_fontKey, m_textureKey, m_text, m_textureWidth, m_textureHeight, m_textColor, m_maxWidth);
}

void UiTextElement::SetMaxWidth(const float amount)
{
    m_maxWidth = amount;
}

void UiTextElement::ComputeFinal()
{
    // Do not use ComputeZoom for UiTextElement. Text size is controlled by the font
    // Be sure to call UiTextElement::ComputePosition after generating the texture with GenerateText
    SetMaxWidth(m_params.scale);
    GenerateText();
    ComputePosition(m_params.xAnchor, m_params.yAnchor);
    SetPadding(m_params.xPadding, m_params.yPadding);
}
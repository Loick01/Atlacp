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

void UiElement::SetParentPosition(const ScreenPosition parentPosition)
{
    m_parentPosition = parentPosition;
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

void UiElement::AddChild(std::unique_ptr<UiElement>& child)
{
    m_childs.push_back(std::move(child));
}

void UiElement::BuildChild(std::unique_ptr<UiElement> child,
    const float scale, const Axis zoomAxis,
    const Anchor xAnchor, const Anchor yAnchor,
    const float paddingScale, const Axis sourceAxis, const Axis paddingAxis)
{
    child->SetParentSize(GetSize());
    child->ComputeFinal(scale, zoomAxis, xAnchor, yAnchor, paddingScale, sourceAxis, paddingAxis);
    AddChild(child);
}

void UiElement::ComputeFinal(
    const float scale, const Axis zoomAxis,
    const Anchor xAnchor, const Anchor yAnchor,
    const float paddingScale, const Axis sourceAxis, const Axis paddingAxis)
{
    ComputeZoom(scale, zoomAxis);
    ComputePosition(xAnchor, yAnchor);
    AddPadding(paddingScale, sourceAxis, paddingAxis);
}

TextArea::TextArea(TextureController& textureController, const std::string& fontFilepath, const SDL_Color color):
    UiElement(textureController), m_textColor(color), m_text("No_Text")
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

void TextArea::ComputeFinal(
    const float scale, const Axis zoomAxis, // zoomAxis is not used
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
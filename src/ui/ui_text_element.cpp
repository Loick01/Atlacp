#include "ui/element/ui_text_element.hpp"

#include "image/texture.hpp"

UiTextElement::UiTextElement(TextureController& textureController, const UiKey& key, const SDL_Color color):
    UiElement(textureController, key), m_textColor(color), m_text("No_Text") // By default, FontSize in DataUi (used for UiTextElement construction) is FontSize::Small
{}

void UiTextElement::SetText(const std::string& text)
{
    m_text = text;
}

void UiTextElement::SetTextSize(const FontSize size)
{
    m_fontSize = size;
}

void UiTextElement::GenerateText() // Same than Drawable::LoadTexture(), will be improved
{
    m_textureKey = m_text; // Will use something else than just text as texture key
    m_textureController.LoadTextureFromText(m_fontSize, m_textureKey, m_text, m_textureWidth, m_textureHeight, m_textColor, m_maxWidth);
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

    for (std::unique_ptr<UiElement>& e : m_childs)
        e->SetParentSize(GetSize());
    
    ComputePosition(m_params.xAnchor, m_params.yAnchor);
    UsePaddingOnPosition(m_params.xPadding, m_params.yPadding);
}
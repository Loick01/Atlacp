#include "ui/ui_component.hpp"

UiComponent::UiComponent(UiController& uiController, const std::string& uiFilepath) :
    m_uiController(uiController), m_uiFilepath(uiFilepath) // Should initialize m_elementKey ?
{}

ElementKey UiComponent::GetKey() const
{
    return m_elementKey;
}

void UiComponent::SetFilepath(const std::string& uiFilepath)
{
    m_uiFilepath = uiFilepath;
}

void UiComponent::Open()
{
    m_uiController.BuildUiFile(m_uiFilepath);
}

void UiComponent::Close()
{
    m_uiController.DeleteElement(m_elementKey);
}
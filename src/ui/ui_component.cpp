#include "ui/ui_component.hpp"

UiComponent::UiComponent(UiController& uiController, const std::string& uiFilepath) :
    m_uiController(uiController), m_uiFilepath(uiFilepath)
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
    const std::vector<ElementKey> created = m_uiController.BuildUiFile(m_uiFilepath);
    m_elementKey = created[0];
}

void UiComponent::Close()
{
    m_uiController.DeleteElement(m_elementKey);
}
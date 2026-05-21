#include "ui_component.hpp"

UiComponent::UiComponent(UiController& uiController, const std::string& uiFilepath) :
    m_uiController(uiController), m_uiFilepath(uiFilepath)
{}

void UiComponent::Open()
{
    m_uiController.BuildUiFile(m_uiFilepath);
}

void UiComponent::Close()
{
    m_uiController.DeleteElement(m_elementKey);
}
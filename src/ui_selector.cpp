#include "ui_selector.hpp"

UiSelector::UiSelector(UiController& uiController) :
    m_uiController(uiController), m_currentIndex(0)
{
    m_elementKey = "selector"; // Remove (should be returned by UiController::BuildUiFile ?)
    m_parentList = {"option0", "option1", "option2", "option3"}; // Remove
    // if () throw std::runtime_error("UiSelector must have at least one key in m_parentList");
}

void UiSelector::Reset()
{
    m_currentIndex = 0;
}

void UiSelector::UpdateParent()
{
    const ElementKey& parentKey = m_parentList[m_currentIndex];
    m_uiController.UpdateParent(m_elementKey, parentKey); 
    m_uiController.GetElement(parentKey)->UpdatePosition(); // Will update the selector position
}

void UiSelector::Previous()
{
    m_currentIndex = (m_currentIndex-1+m_parentList.size())%m_parentList.size();
    UpdateParent();
}

void UiSelector::Next()
{
    m_currentIndex = (m_currentIndex+1)%m_parentList.size();   
    UpdateParent();
}
#include "ui/ui_selector.hpp"

UiSelector::UiSelector(UiController& uiController, const std::string& uiFilepath) :
    UiComponent(uiController, uiFilepath), m_currentIndex(0)
{
    m_elementKey = "selector"; // Remove (should be returned by UiController::BuildUiFile ?)
}

int UiSelector::GetIndex() const
{
    return m_currentIndex;
}

void UiSelector::Reset()
{
    m_currentIndex = 0;
    UpdateParent();
}

void UiSelector::SetParents(std::initializer_list<ElementKey> parents)
{
    if (parents.size() == 0) throw std::runtime_error("UiSelector must have at least one key in m_parents");
    m_parents = parents;
    Reset(); 
}

void UiSelector::UpdateParent()
{
    const ElementKey& parentKey = m_parents[m_currentIndex];
    m_uiController.UpdateParent(m_elementKey, parentKey); 
    m_uiController.GetElement(parentKey)->UpdatePosition(); // Will update the selector position
}

void UiSelector::Previous()
{
    m_currentIndex = (m_currentIndex-1+m_parents.size())%m_parents.size();
    UpdateParent();
}

void UiSelector::Next()
{
    m_currentIndex = (m_currentIndex+1)%m_parents.size();   
    UpdateParent();
}
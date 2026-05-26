#include "ui/ui_list.hpp"

UiList::UiList(UiController& uiController, const std::string& uiFilepath) :
    UiComponent(uiController, uiFilepath)
{
    m_elements = {"option0", "option1", "option2", "option3"}; // Remove (should be returned by UiController::BuildUiFile ?)
}

std::vector<ElementKey> UiList::GetElementsKey() const
{
    return m_elements;
}
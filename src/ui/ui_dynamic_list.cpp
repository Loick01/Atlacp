#include "ui/ui_dynamic_list.hpp"

UiDynamicList::UiDynamicList(UiController& uiController, const std::string& uiFilepath):
    UiList(uiController, uiFilepath)
{}

void UiDynamicList::SetNrItem(const unsigned int nrItem)
{
    m_nrItem = nrItem;
}
        
// void Open()
// {

// }
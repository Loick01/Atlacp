#include "ui/ui_dynamic_list.hpp"

unsigned int UiDynamicList::m_instanceCount = 0;

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

unsigned int UiDynamicList::GetInstanceCount()
{
    return m_instanceCount++;
}
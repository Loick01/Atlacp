#include "ui/ui_list.hpp"

UiList::UiList(UiController& uiController, const std::string& uiFilepath) :
    UiComponent(uiController, uiFilepath)
{}

std::vector<ElementKey> UiList::GetItemsKey() const
{
    return m_itemsKey;
}

void UiList::Open()
{
    const std::vector<ElementKey> created = m_uiController.BuildUiFile(m_uiFilepath);
    m_elementKey = created[0];
    m_itemsKey = created;
}

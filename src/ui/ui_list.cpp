#include "ui/ui_list.hpp"

unsigned int UiList::m_instanceCount = 0;

UiList::UiList(UiController& uiController, const std::string& uiFilepath) :
    UiComponent(uiController, uiFilepath)
{}

std::vector<ElementKey> UiList::GetItemsKey() const
{
    return m_itemsKey;
}

void UiList::Open()
{
    // Should do the same process than UiComponent::Open() ?
    const std::vector<ElementKey> created = m_uiController.BuildUiFile(m_uiFilepath);
    m_elementKey = created[0];

    m_itemsKey = created;
}

unsigned int UiList::GetInstanceCount()
{
    return m_instanceCount++;
}

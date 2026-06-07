#include "ui/ui_list.hpp"

#include "ui/ui_controller.hpp"

UiList::UiList(UiController& uiController, const std::string& uiFilepath) :
    UiComponent(uiController, uiFilepath)
{
    // if (!m_uiController.IsBaseUiFile(m_uiFilepath))
    //     throw std::runtime_error("The file used for UiList must be a base file : " + m_uiFilepath);
}

std::vector<ElementKey> UiList::GetItemsKey() const
{
    return m_itemsKey;
}

void UiList::Open()
{
    // m_uiFilepath is a .uif
    const std::vector<ElementKey> created = m_uiController.BuildUiFile(m_uiFilepath);
    m_elementKey = created[0];
    m_itemsKey = created;
}

unsigned int UiList::GetInstanceCount()
{
    return m_instanceCount++;
}

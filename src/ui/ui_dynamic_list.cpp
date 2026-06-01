#include "ui/ui_dynamic_list.hpp"

#include "ui/ui_controller.hpp"

unsigned int UiDynamicList::m_instanceCount = 0;

UiDynamicList::UiDynamicList(UiController& uiController, const std::string& uiFilepath):
    UiList(uiController, uiFilepath)
{
    if (!m_uiController.IsTemplateUiFile(m_uiFilepath))
        throw std::runtime_error("The file used for UiDynamicList must be a template file : " + m_uiFilepath);
}

unsigned int UiDynamicList::GetInstanceCount()
{
    return m_instanceCount++;
}
        
void UiDynamicList::Open()
{   
    if (m_nrItem == 0) throw std::runtime_error("UiDynamicList::m_nrItem should not be 0");
     
    const unsigned int countInstance = GetInstanceCount();
    ElementKey lastItemParent;
    
    for (unsigned int i = 0 ; i < m_nrItem ; i++) {
        std::vector<ElementKey> created = m_uiController.BuildUiFile(m_uiFilepath);
        
        for (ElementKey& k : created) {
            const ElementKey newKey = k + std::to_string(i) + "_" + std::to_string(countInstance);
            m_uiController.UpdateKey(k, newKey);
            k = newKey;
        }
        const ElementKey& itemParent = created[0];
        
        if (i == 0) {
            m_elementKey = itemParent;
            m_uiController.UpdateParams(itemParent, m_firstItemParams);
            // Parent of itemParent (which is here parent of the first built item) will be the one given in the template file (should not be like that ?) 
        } else {
            m_uiController.UpdateParent(itemParent, lastItemParent);
        }
        m_itemsKey.push_back(itemParent);
        lastItemParent = itemParent;
    }
}

void UiDynamicList::Close()
{
    UiComponent::Close();
    m_itemsKey.clear();
}

void UiDynamicList::SetFirstItemParams(const UiParams& params)
{
    m_firstItemParams = params;
}

void UiDynamicList::SetNrItem(const unsigned int nrItem)
{
    m_nrItem = nrItem;
}
#include "ui/ui_dynamic_list.hpp"

unsigned int UiDynamicList::m_instanceCount = 0;

UiDynamicList::UiDynamicList(UiController& uiController, const std::string& uiFilepath):
    UiList(uiController, uiFilepath)
{
    // Get the file extension (will be in FileReader)
    size_t pos = m_uiFilepath.rfind('.');
    if (pos == std::string::npos)
        throw std::runtime_error("This ui file has no extension : " + m_uiFilepath);
    const std::string fileExtension = m_uiFilepath.substr(pos + 1);
    
    if (fileExtension == "uif") 
        throw std::runtime_error("Do not use ui file for UiDynamicList items, it can be only instantiated once : " + m_uiFilepath);
}

void UiDynamicList::SetNrItem(const unsigned int nrItem)
{
    m_nrItem = nrItem;
}
        
void UiDynamicList::Open()
{   
    if (m_nrItem == 0) throw std::runtime_error("UiDynamicList::m_nrItem should not be 0");
     
    const unsigned int countInstance = GetInstanceCount();
    
    for (unsigned int i = 0 ; i < m_nrItem ; i++) {
        std::vector<ElementKey> created = m_uiController.BuildUiFile(m_uiFilepath);
        
        for (ElementKey& k : created) {
            const ElementKey newKey = k + std::to_string(i) + "_" + std::to_string(countInstance);
            m_uiController.UpdateKey(k, newKey);
            k = newKey;
            std::cout << k << "\n";
        }
        if (i == 0) m_elementKey = created[0];
        m_itemsKey.push_back(created[0]);
    }
}

void UiDynamicList::Close()
{
    UiComponent::Close();
    m_itemsKey.clear();
}

unsigned int UiDynamicList::GetInstanceCount()
{
    return m_instanceCount++;
}
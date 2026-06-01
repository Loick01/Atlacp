#include "ui/ui_component.hpp"

#include "ui/ui_controller.hpp"

UiComponent::UiComponent(UiController& uiController, const std::string& uiFilepath) :
    m_uiController(uiController), m_uiFilepath(uiFilepath)
{}

ElementKey UiComponent::GetKey() const
{
    return m_elementKey;
}

void UiComponent::Open()
{
    std::vector<ElementKey> created = m_uiController.BuildUiFile(m_uiFilepath);

    if (m_uiController.IsTemplateUiFile(m_uiFilepath)) {
        const unsigned int countInstance = GetInstanceCount();
        // That's why template file should not have load command (BuildUiFile could return keys only for elements constructed in the file ?)
        // Or I could use a function similar to BuildUiFile ?
        for (ElementKey& k : created) {
            const ElementKey newKey = k + std::to_string(countInstance);
            m_uiController.UpdateKey(k, newKey);
            k = newKey; // Necessary only the first key (the one used for m_elementKey below)  
        }
    }
    
    m_elementKey = created[0];
}

void UiComponent::Close()
{
    m_uiController.DeleteElement(m_elementKey);
}
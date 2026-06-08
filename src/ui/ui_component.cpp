#include "ui/ui_component.hpp"

#include "ui/ui_controller.hpp"

UiComponent::UiComponent(UiController& uiController, const std::string& uiFilepath) :
    m_uiController(uiController), m_uiFilepath(uiFilepath)
{}

UiKey UiComponent::GetKey() const
{
    return m_uiKey;
}

void UiComponent::Open()
{
    std::vector<UiKey> created = m_uiController.BuildUiFile(m_uiFilepath);

    if (m_uiController.IsTemplateUiFile(m_uiFilepath)) {
        const unsigned int countInstance = GetInstanceCount();
        // That's why template file should not have load command (BuildUiFile could return keys only for elements constructed in the file ?)
        // Or I could use a function similar to BuildUiFile ?
        for (UiKey& k : created) {
            const UiKey newKey = k + std::to_string(countInstance);
            m_uiController.UpdateKey(k, newKey);
            k = newKey; // Necessary only for the first key (the one used for m_uiKey below)  
        }
    }
    
    m_uiKey = created[0];
}

void UiComponent::Close()
{
    m_uiController.DeleteElement(m_uiKey);
}
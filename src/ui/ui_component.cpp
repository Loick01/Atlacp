#include "ui/ui_component.hpp"

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

    if (m_uiController.IsTemplateFile(m_uiFilepath)) {
        const unsigned int countInstance = GetInstanceCount();
        for (ElementKey& k : created) {
            const ElementKey newKey = k + std::to_string(countInstance);
            m_uiController.UpdateKey(k, newKey);
            k = newKey;
        }
    }
    
    m_elementKey = created[0];
}

void UiComponent::Close()
{
    m_uiController.DeleteElement(m_elementKey);
}
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

    // Get the file extension (will be in FileReader)
    size_t pos = m_uiFilepath.rfind('.');
    if (pos == std::string::npos)
        throw std::runtime_error("This ui file has no extension : " + m_uiFilepath);
    const std::string fileExtension = m_uiFilepath.substr(pos + 1);

    if (fileExtension == "uit") {
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
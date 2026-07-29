#include "ui/component/ui_animated.hpp"

#include "ui/ui_controller.hpp"

UiAnimated::UiAnimated(UiController& uiController, const std::string& uiFilepath) :
    UiComponent(uiController, uiFilepath)
{}

void UiAnimated::Open()
{
    UiComponent::Open();

    // Should not be in Open() ?
    m_uiController.UpdateParent(m_uiKey, m_targetElement);
    m_uiController.UpdateScalingSize(m_uiKey, PartialSize{m_targetElement, Axis::Width, 0.8f}); 
}

void UiAnimated::SetTargetElement(const UiKey& key)
{
    m_targetElement = key;
}

unsigned int UiAnimated::GetInstanceCount()
{
    return m_instanceCount++;
}
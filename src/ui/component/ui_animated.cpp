#include "ui/component/ui_animated.hpp"

#include "core/time.hpp"
#include "ui/ui_animated_element.hpp"
#include "ui/ui_controller.hpp"

UiAnimated::UiAnimated(const Time& time, UiController& uiController, const std::string& uiFilepath) :
    UiComponent(uiController, uiFilepath), m_time(time), m_animated(nullptr)
{}

bool UiAnimated::IsDone() const
{
    return m_animated->GetAnimation().IsDone();
}

void UiAnimated::Open()
{
    UiComponent::Open();
    // Need to check if a UiAnimatedElement is read in the ui file ?
    
    // Should not be in Open() ?
    m_uiController.UpdatePath(m_uiKey, "../assets/battle/actor_placeholder.png");
    m_uiController.UpdateParent(m_uiKey, m_targetElement);
    m_uiController.UpdateScalingSize(m_uiKey, PartialSize{m_targetElement, Axis::Width, 0.8f});

    m_animated = dynamic_cast<UiAnimatedElement*>(m_uiController.GetElement(m_uiKey)); // static_cast ?
    if (m_animated == nullptr) throw std::runtime_error("This UiElement must be a UiAnimatedElement");
}

void UiAnimated::SetTargetElement(const UiKey& key)
{
    m_targetElement = key;
}

void UiAnimated::ContinueAnimation()
{
    m_animated->GetAnimation().Continue(m_time.GetDeltaTime());
}

unsigned int UiAnimated::GetInstanceCount()
{
    return m_instanceCount++;
}
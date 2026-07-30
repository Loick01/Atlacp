#include "ui/component/ui_animated.hpp"

#include "core/time.hpp"
#include "ui/ui_animated_element.hpp"
#include "ui/ui_controller.hpp"

UiAnimated::UiAnimated(const Time& time, UiController& uiController, const std::string& uiFilepath) :
    UiComponent(uiController, uiFilepath), m_time(time), m_animated(nullptr), m_animationPath("invalid")
{}

bool UiAnimated::IsDone() const
{
    return m_animated->GetAnimation().IsDone();
}

void UiAnimated::Open()
{
    UiComponent::Open();
    // Need to check if a UiAnimatedElement is read in the ui file ?
    
    // m_uiController.UpdatePath(m_uiKey, "../assets/battle/move_animation/"+m_animationPath+".png"); // TODO
    // UpdatePath calls Drawable::LoadTexture and will write on m_textureWidth and m_textureHeight
    m_animated = dynamic_cast<UiAnimatedElement*>(m_uiController.GetElement(m_uiKey)); // static_cast ?
    if (m_animated == nullptr) throw std::runtime_error("This UiElement must be a UiAnimatedElement");
    m_animated->GetSpriteSizeFromAnimation(); // Must be called after UpdatePath() and before UpdateScalingSize()
    
    m_uiController.UpdateParent(m_uiKey, m_targetElement);
    m_uiController.UpdateScalingSize(m_uiKey, PartialSize{m_targetElement, Axis::Width, 0.8f});
}

void UiAnimated::SetAnimationPath(const std::string animationPath)
{
    m_animationPath = animationPath;
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
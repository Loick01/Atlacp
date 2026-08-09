#include "ui/component/ui_sprite_animation.hpp"

#include "core/path.hpp"
#include "core/time.hpp"
#include "ui/element/ui_animated_element.hpp"
#include "ui/element/ui_controller.hpp"

UiSpriteAnimation::UiSpriteAnimation(const Time& time, UiController& uiController, const std::string& uiFilepath) :
    UiComponent(uiController, uiFilepath), m_time(time), m_animated(nullptr), m_animationPath("invalid")
{}

bool UiSpriteAnimation::IsDone() const
{
    return m_animated->GetAnimation().IsDone();
}

void UiSpriteAnimation::Open()
{
    UiComponent::Open();
    // Need to check if a UiAnimatedElement is read in the ui file ?
    
    // UpdatePath calls Drawable::LoadTexture and will write on m_textureWidth and m_textureHeight
    // That's why I must call UiAnimatedElement::UpdateSpriteSizeFromAnimation
    m_uiController.UpdatePath(m_uiKey, AssetDirectory::Spritesheet+m_animationPath);
    m_animated = dynamic_cast<UiAnimatedElement*>(m_uiController.GetElement(m_uiKey)); // static_cast ?
    if (m_animated == nullptr) throw std::runtime_error("This UiElement must be a UiAnimatedElement");
    m_animated->GetAnimation().UpdateAnimationData(m_animationPath); // Must be called before UpdateSpriteSizeFromAnimation
    m_animated->UpdateSpriteSizeFromAnimation(); // Must be called after UpdatePath() and before UpdateScalingSize()
    
    m_uiController.UpdateParent(m_uiKey, m_targetElement);
    m_uiController.UpdateScalingSize(m_uiKey, PartialSize{m_targetElement, Axis::Width, 0.8f});
}

void UiSpriteAnimation::SetAnimationPath(const std::string animationPath)
{
    m_animationPath = animationPath;
}

void UiSpriteAnimation::SetTargetElement(const UiKey& key)
{
    m_targetElement = key;
}

void UiSpriteAnimation::ContinueAnimation()
{
    m_animated->GetAnimation().Continue(m_time.GetDeltaTime());
}

unsigned int UiSpriteAnimation::GetInstanceCount()
{
    return m_instanceCount++;
}

void UiSpriteAnimation::ResetInstanceCount()
{
    m_instanceCount = 0;
}
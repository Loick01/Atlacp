#include "ui/component/ui_selector.hpp"

#include "sound/sound.hpp"
#include "ui/element/ui_controller.hpp"

UiSelector::UiSelector(UiController& uiController, const std::string& uiFilepath) :
    UiComponent(uiController, uiFilepath), m_optionIndex(0)
{}

int UiSelector::GetOptionIndex() const
{
    return m_optionIndex;
}

bool UiSelector::VerticalNavigation(const Direction d, const bool isAction)
{
    if (isAction) {
        SoundController::GetInstance().RequestChunk(BaseSfx::Accept);
        return true;
    } else if (d == Direction::Down) {
        SoundController::GetInstance().RequestChunk(BaseSfx::Move); // Should be in Next() ?
        Next();
    } else if (d == Direction::Up) {
        SoundController::GetInstance().RequestChunk(BaseSfx::Move); // Should be in Previous() ?
        Previous();
    }
    return false;
}

bool UiSelector::HorizontalNavigation(const Direction d, const bool isAction)
{
    if (isAction) {
        SoundController::GetInstance().RequestChunk(BaseSfx::Accept);
        return true;
    } else if (d == Direction::Right) {
        SoundController::GetInstance().RequestChunk(BaseSfx::Move); // Should be in Next() ?
        Next();
    } else if (d == Direction::Left) {
        SoundController::GetInstance().RequestChunk(BaseSfx::Move); // Should be in Previous() ?
        Previous();
    }
    return false;
}

void UiSelector::UpdateToOptionIndex()
{
    const UiKey& parentKey = m_optionKeys[m_optionIndex];
    m_uiController.UpdateParent(m_uiKey, parentKey); 
    m_uiController.GetElement(parentKey)->UpdatePosition();
}

void UiSelector::Reset()
{
    m_optionIndex = 0;
    UpdateToOptionIndex();
}

void UiSelector::SetOptionKeys(const std::vector<UiKey>& optionKeys, const Axis scaleAxis, const Axis padAxis, const float scaleValue, const float padValue)
{
    if (optionKeys.size() == 0) throw std::runtime_error("UiSelector must have at least one key in m_optionKeys");
    m_optionKeys = optionKeys;
    Reset(); 
    // When selector file is build, scale is based on root element, and there is no padding
    m_uiController.UpdateScalingSize(GetKey(), PartialSize{optionKeys[0], scaleAxis, scaleValue}); 
    m_uiController.UpdatePadding(GetKey(), PartialSize(optionKeys[0], padAxis, padValue), padAxis);
}

void UiSelector::Previous()
{
    m_optionIndex = (m_optionIndex-1+m_optionKeys.size())%m_optionKeys.size();
    UpdateToOptionIndex();
}

void UiSelector::Next()
{
    m_optionIndex = (m_optionIndex+1)%m_optionKeys.size();   
    UpdateToOptionIndex();
}

unsigned int UiSelector::GetInstanceCount()
{
    return m_instanceCount++;
}

void UiSelector::ResetInstanceCount()
{
    m_instanceCount = 0;
}
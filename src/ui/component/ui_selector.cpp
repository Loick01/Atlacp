#include "ui/component/ui_selector.hpp"

#include "core/logger.hpp"
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
        SoundController::GetInstance().RequestChunk(std::string(BaseSfx::Accept));
        return true;
    } else if (d == Direction::Down) {
        Next();
    } else if (d == Direction::Up) {
        Previous();
    }
    return false;
}

bool UiSelector::HorizontalNavigation(const Direction d, const bool isAction)
{
    if (isAction) {
        SoundController::GetInstance().RequestChunk(std::string(BaseSfx::Accept));
        return true;
    } else if (d == Direction::Right) {
        Next();
    } else if (d == Direction::Left) {
        Previous();
    }
    return false;
}

void UiSelector::UpdateToOptionIndex()
{
    if (!(m_optionIndex >= 0 && m_optionIndex < m_optionKeys.size())) {
        Logger::GetInstance().AddLog(Level::L4, "(UiSelector::UpdateToOptionIndex) Invalid index used with this selector: " + m_uiKey); // TODO : Level
        return;
    }
    const UiKey& parentKey = m_optionKeys.at(m_optionIndex);
    m_uiController.EditParent(m_uiKey, parentKey); 
    m_uiController.GetElement(parentKey)->UpdatePosition();
}

void UiSelector::Reset()
{
    m_optionIndex = 0;
    UpdateToOptionIndex();
}

void UiSelector::SetOptionKeys(const std::vector<UiKey>& optionKeys, const Axis scaleAxis, const Axis padAxis, const float scaleValue, const float padValue)
{
    if (optionKeys.empty()) {
        Logger::GetInstance().AddLog(Level::L4, "UiSelector must have at least one key in m_optionKeys"); // TODO : Level
        m_optionKeys.clear(); // Otherwise, it will keep the previous keys
        return;
    }
    
    m_optionKeys = optionKeys;
    Reset(); 
    // When selector file is build, scale is based on root element, and there is no padding
    m_uiController.EditScalingSize(GetKey(), PartialSize{optionKeys[0], scaleAxis, scaleValue}); 
    m_uiController.EditPadding(GetKey(), PartialSize(optionKeys[0], padAxis, padValue), padAxis);
}

void UiSelector::Previous()
{
    if (m_optionKeys.empty()) {
        Logger::GetInstance().AddLog(Level::L4, "(UiSelector::Previous) No options for this selector: " + m_uiKey); // TODO : Level
        return;
    }
    m_optionIndex = (m_optionIndex-1+m_optionKeys.size())%m_optionKeys.size();
    UpdateToOptionIndex();
    SoundController::GetInstance().RequestChunk(std::string(BaseSfx::Move));
}

void UiSelector::Next()
{
    if (m_optionKeys.empty()) {
        Logger::GetInstance().AddLog(Level::L4, "(UiSelector::Next) No options for this selector: " + m_uiKey); // TODO : Level
        return;
    }
    m_optionIndex = (m_optionIndex+1)%m_optionKeys.size();   
    UpdateToOptionIndex();
    SoundController::GetInstance().RequestChunk(std::string(BaseSfx::Move));
}

unsigned int UiSelector::GetInstanceCount()
{
    return m_instanceCount++;
}

void UiSelector::ResetInstanceCount()
{
    m_instanceCount = 0;
}
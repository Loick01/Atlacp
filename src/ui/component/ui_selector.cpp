#include "ui/component/ui_selector.hpp"

#include "sound/sound.hpp"
#include "ui/ui_controller.hpp"

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

void UiSelector::SetOptionKeys(std::vector<UiKey> optionKeys)
{
    if (optionKeys.size() == 0) throw std::runtime_error("UiSelector must have at least one key in m_optionKeys");
    m_optionKeys = optionKeys;
    Reset(); 
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
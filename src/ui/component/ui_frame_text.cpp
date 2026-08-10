#include "ui/component/ui_frame_text.hpp"

#include "ui/element/ui_controller.hpp"

namespace { // These values must be the same as in the ui file used for UiFrameText (example : frame_text.uif)
    // Should be constexpr std::string_view ?
    const std::string keyFrameText = "frameText";
}

UiFrameText::UiFrameText(UiController& uiController, const std::string& uiFilepath) :
    UiComponent(uiController, uiFilepath)
{}

bool UiFrameText::NextText()
{
    if (m_texts.empty()) return false;
    m_uiController.UpdateText(keyFrameText, m_texts.front());
    m_texts.pop();
    return true;
}

void UiFrameText::AddText(const std::vector<std::string>& texts)
{
    for (const std::string& s : texts)
        m_texts.push(s);
}

unsigned int UiFrameText::GetInstanceCount()
{
    return m_instanceCount++;
}

void UiFrameText::ResetInstanceCount()
{
    m_instanceCount = 0;
}
#include "ui/component/ui_frame_text.hpp"

#include "ui/element/ui_controller.hpp"

UiFrameText::UiFrameText(UiController& uiController, const std::string& uiFilepath) :
    UiComponent(uiController, uiFilepath)
{}

bool UiFrameText::NextText()
{
    if (m_texts.empty()) return false;
    m_uiController.UpdateText(m_uiKey, m_texts.front());
    m_texts.pop();
    return true;
}

void UiFrameText::AddText(std::initializer_list<std::string> texts)
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
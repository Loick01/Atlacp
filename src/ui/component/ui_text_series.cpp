#include "ui/component/ui_text_series.hpp"

#include "ui/element/ui_controller.hpp"

UiTextSeries::UiTextSeries(UiController& uiController, const std::string& uiFilepath) :
    UiComponent(uiController, uiFilepath)
{}

bool UiTextSeries::NextText()
{
    if (m_texts.empty()) return false;
    m_uiController.UpdateText(m_uiKey, m_texts.front());
    m_texts.pop();
    return true;
}

void UiTextSeries::AddText(std::initializer_list<std::string> texts)
{
    for (const std::string& s : texts)
        m_texts.push(s);
}

unsigned int UiTextSeries::GetInstanceCount()
{
    return m_instanceCount++;
}

void UiTextSeries::ResetInstanceCount()
{
    m_instanceCount = 0;
}
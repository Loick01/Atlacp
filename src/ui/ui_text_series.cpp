#include "ui/ui_text_series.hpp"

UiTextSeries::UiTextSeries(UiController& uiController, const std::string& uiFilepath) :
    UiComponent(uiController, uiFilepath)
{
    m_elementKey = "frameText"; // Remove (should be returned by UiController::BuildUiFile ?)
}

bool UiTextSeries::Next()
{
    if (m_texts.empty()) return false;
    m_uiController.UpdateText(m_elementKey, m_texts.front());
    m_texts.pop();
    return true;
}

void UiTextSeries::AddText(std::initializer_list<std::string> texts)
{
    for (const std::string& s : texts)
        m_texts.push(s);
    Next(); // Should not be here ?
}
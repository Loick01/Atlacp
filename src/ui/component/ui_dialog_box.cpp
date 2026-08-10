#include "ui/component/ui_dialog_box.hpp"

#include "ui/element/ui_controller.hpp"

UiDialogBox::UiDialogBox(UiController& uiController, const std::string& uiFilepath) :
    UiComponent(uiController, uiFilepath)
{}

void UiDialogBox::SetText(const std::string& text)
{
    m_uiController.UpdateText("dialogText", text);
}

void UiDialogBox::SetFacePath(const std::string& facePath)
{
    m_uiController.UpdatePath("face", facePath);
}

unsigned int UiDialogBox::GetInstanceCount()
{
    return m_instanceCount++;
}

void UiDialogBox::ResetInstanceCount()
{
    m_instanceCount = 0;
}
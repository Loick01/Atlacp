#include "ui/component/ui_dialog_box.hpp"

#include "ui/element/ui_controller.hpp"

namespace { // These values must be the same as in the ui file used for UiDialogBox (example : dialog_box.uif)
    // Should be constexpr std::string_view ?
    const std::string keyDialogText = "dialogText";
    const std::string keyFace = "face";
}

UiDialogBox::UiDialogBox(UiController& uiController, const std::string& uiFilepath) :
    UiComponent(uiController, uiFilepath)
{}

void UiDialogBox::SetText(const std::string& text)
{
    m_uiController.UpdateText(keyDialogText, text);
}

void UiDialogBox::SetFacePath(const std::string& facePath)
{
    m_uiController.UpdatePath(keyFace, facePath);
}

unsigned int UiDialogBox::GetInstanceCount()
{
    return m_instanceCount++;
}

void UiDialogBox::ResetInstanceCount()
{
    m_instanceCount = 0;
}
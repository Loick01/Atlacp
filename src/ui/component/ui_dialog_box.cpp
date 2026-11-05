#include "ui/component/ui_dialog_box.hpp"

#include "ui/element/ui_controller.hpp"

namespace { // These values must be the same as in the ui file used for UiDialogBox (example : dialog_box.uif)
    // For dialog box, the ui key to access the text is in the namespace in ui_frame_text.cpp
    inline constexpr std::string_view keyFace = "face";
}

UiDialogBox::UiDialogBox(UiController& uiController, const std::string& uiFilepath) :
    UiFrameText(uiController, uiFilepath)
{}

void UiDialogBox::SetFacePath(const std::string& facePath)
{
    m_uiController.UpdatePath(std::string(keyFace), facePath);
}
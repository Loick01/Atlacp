#pragma once

#include "ui/component/ui_frame_text.hpp"
#include "ui/ui_types.hpp"

class UiDialogBox : public UiFrameText
{   
    // Currently, UiDialog shares m_instanceCount with UiFrameText, but shouldn't it have its own ?
    
    public:
        UiDialogBox(UiController& uiController, const std::string& uiFilepath);
        
        void SetFacePath(const std::string& facePath);
};
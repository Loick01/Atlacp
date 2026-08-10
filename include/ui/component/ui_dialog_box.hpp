#pragma once

#include "ui/component/ui_component.hpp"
#include "ui/ui_types.hpp"

class UiDialogBox : public UiComponent // Will inherits from TextSeries ?
{
    private:
        static inline unsigned int m_instanceCount;
        // std::queue<std::string> m_texts;
    
    public:
        UiDialogBox(UiController& uiController, const std::string& uiFilepath);

        unsigned int GetInstanceCount() override;
        void ResetInstanceCount() override;
        
        void SetText(const std::string& text);
        void SetFacePath(const std::string& facePath);
};
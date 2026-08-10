#pragma once

#include <initializer_list>
#include <queue>

#include "ui/component/ui_component.hpp"
#include "ui/ui_types.hpp"

class UiFrameText : public UiComponent
{
    private:
        static inline unsigned int m_instanceCount;
        std::queue<std::string> m_texts;
    
    public:
        UiFrameText(UiController& uiController, const std::string& uiFilepath);

        unsigned int GetInstanceCount() override;
        void ResetInstanceCount() override;
        
        bool NextText(); // Must be called after texts have been added (but I can't do it in AddText)
        void AddText(const std::vector<std::string>& texts);
};
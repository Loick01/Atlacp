#pragma once

#include <initializer_list>
#include <queue>

#include "ui/component/ui_component.hpp"
#include "ui/ui_types.hpp"

class UiTextSeries : public UiComponent
{
    private:
        static inline unsigned int m_instanceCount;
        std::queue<std::string> m_texts;
    
    public:
        UiTextSeries(UiController& uiController, const std::string& uiFilepath);

        unsigned int GetInstanceCount() override;
        
        bool NextText(); // Must be called after texts have been added (but I can't do it in AddText)
        void AddText(std::initializer_list<std::string> texts);
};
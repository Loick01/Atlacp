#pragma once

#include <initializer_list>
#include <queue>

#include "ui/ui_component.hpp"
#include "ui/ui_controller.hpp"
#include "ui/ui_element.hpp"

class UiTextSeries : public UiComponent
{
    private:
        static unsigned int m_instanceCount;
        std::queue<std::string> m_texts;
    
    public:
        UiTextSeries(UiController& uiController, const std::string& uiFilepath);

        unsigned int GetInstanceCount() override;
        
        bool Next();
        void AddText(std::initializer_list<std::string> texts);
};
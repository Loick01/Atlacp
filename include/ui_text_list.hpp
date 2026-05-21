#pragma once

#include <initializer_list>
#include <queue>

#include "ui_component.hpp"
#include "ui_controller.hpp"
#include "ui_element.hpp"

class UiTextList : public UiComponent
{
    private:
        std::queue<std::string> m_texts;
    
    public:
        UiTextList(UiController& uiController, const std::string& uiFilepath);

        bool Next(); // Rename
        void AddText(std::initializer_list<std::string> texts);
};
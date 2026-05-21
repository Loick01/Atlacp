#pragma once

#include <initializer_list>
#include <queue>

#include "ui_controller.hpp"
#include "ui_element.hpp"

class UiTextList // Will inherits UiComponent
{
    private:
        UiController& m_uiController; // Will be in UiComponent
        std::queue<std::string> m_texts;
        ElementKey m_elementKey; // Will be in UiComponent
        // std::string m_uiFilepath; // Will be in UiComponent
    
    public:
        UiTextList(UiController& uiController);

        bool Next(); // Rename
        void AddText(std::initializer_list<std::string> texts);
};
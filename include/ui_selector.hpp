#pragma once

#include "ui_controller.hpp"
#include "ui_element.hpp"

class UiSelector // Should create UiComponent ?
{
    private:
        UiController* m_uiController;
        std::vector<ElementKey> m_parentList;
        ElementKey m_elementKey;
        int m_currentIndex;
    
    public:
        UiSelector();

        void SetUiController(UiController* uiController); // Should not be here
        void UpdateParent();
        void Previous();
        void Next();
};
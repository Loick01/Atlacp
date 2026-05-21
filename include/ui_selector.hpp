#pragma once

#include "ui_controller.hpp"
#include "ui_element.hpp"

class UiSelector // Will inherits UiComponent
{
    private:
        UiController& m_uiController; // Will be in UiComponent
        std::vector<ElementKey> m_parentList;
        ElementKey m_elementKey; // Will be in UiComponent
        // std::string m_uiFilepath; // Will be in UiComponent
        int m_currentIndex;
    
    public:
        UiSelector(UiController& uiController);

        int GetIndex() const;
        void Reset();
        void UpdateParent();
        void Previous();
        void Next();
};
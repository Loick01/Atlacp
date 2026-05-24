#pragma once

#include <initializer_list>

#include "ui_component.hpp"
#include "ui_controller.hpp"
#include "ui_element.hpp"

class UiSelector : public UiComponent
{
    private:
        std::vector<ElementKey> m_parents;
        int m_currentIndex;
    
    public:
        UiSelector(UiController& uiController, const std::string& uiFilepath);

        int GetIndex() const;
        void Reset();
        void SetParents(std::initializer_list<ElementKey> parents);
        void UpdateParent();
        void Previous();
        void Next();
};
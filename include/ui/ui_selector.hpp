#pragma once

#include <initializer_list>

#include "ui/ui_component.hpp"
#include "ui/ui_controller.hpp"
#include "ui/ui_element.hpp"

class UiSelector : public UiComponent
{
    private:
        static unsigned int m_instanceCount;
        std::vector<ElementKey> m_parents; // Rename
        int m_currentIndex;
    
    public:
        UiSelector(UiController& uiController, const std::string& uiFilepath);

        unsigned int GetInstanceCount() override;
        
        int GetIndex() const;
        void Reset();
        void SetParents(std::vector<ElementKey> parents);
        void UpdateParent();
        void Previous();
        void Next();
};
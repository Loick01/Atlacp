#pragma once

#include <initializer_list>
#include <string>
#include <vector>

#include "ui/ui_component.hpp"
#include "ui/ui_types.hpp" // UiKey

class UiSelector : public UiComponent
{
    private:
        static inline unsigned int m_instanceCount;
        std::vector<UiKey> m_parents; // Rename
        int m_currentIndex;
    
    public:
        UiSelector(UiController& uiController, const std::string& uiFilepath);

        unsigned int GetInstanceCount() override;
        
        int GetIndex() const;
        void Reset();
        void SetParents(std::vector<UiKey> parents);
        void UpdateParent();
        void Previous();
        void Next();
};
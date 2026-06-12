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
        std::vector<UiKey> m_optionKeys;
        int m_optionIndex;

        void UpdateToOptionIndex(); // Use m_optionIndex
    
    public:
        UiSelector(UiController& uiController, const std::string& uiFilepath);

        unsigned int GetInstanceCount() override;
        
        int GetOptionIndex() const;
        void Reset();
        void SetOptionKeys(std::vector<UiKey> optionKeys);
        void Previous();
        void Next();
};
#pragma once

#include "ui/component/ui_component.hpp"

class UiAnimated : public UiComponent
{
    private:
        static inline unsigned int m_instanceCount;

        UiKey m_targetElement;
    
    public:
        UiAnimated(UiController& uiController, const std::string& uiFilepath);

        unsigned int GetInstanceCount() override;
        
        void Open() override;
        void SetTargetElement(const UiKey& key);
};
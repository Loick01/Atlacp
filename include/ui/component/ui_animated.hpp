#pragma once

#include "ui/component/ui_component.hpp"

struct Time;
class UiAnimatedElement;

class UiAnimated : public UiComponent
{
    private:
        static inline unsigned int m_instanceCount;

        const Time& m_time; // Should not be here, and have only deltaTime from BattleController ?
        UiKey m_targetElement; // Key of the UiElement that will be the parent of the UiAnimatedElement
        UiAnimatedElement* m_animated; // Should have instead a reference to UiAnimatedElement::m_animation ?
    
    public:
        UiAnimated(const Time& time, UiController& uiController, const std::string& uiFilepath);

        unsigned int GetInstanceCount() override;
        
        bool IsDone() const;
        void Open() override;
        void SetTargetElement(const UiKey& key);
        void ContinueAnimation();
};
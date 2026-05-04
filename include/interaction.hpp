#pragma once

#include "entity.hpp"
#include "type.hpp"
#include "ui_controller.hpp"

class InteractionController
{
    private:
        UiController* m_uiController; // GameplayUiController only ?
        Entity* m_srcEntity;
        Entity* m_dstEntity;
    
    public: 
        InteractionController();

        void SetUiController(UiController* uiController);
        void StartInteraction(std::vector<Entity*> entities);
};
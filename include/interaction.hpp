#pragma once

#include "entity.hpp"
#include "type.hpp"
#include "ui_controller.hpp"

class InteractionController
{
    private:
        UiController& m_uiController;
        Entity* m_srcEntity;
        Entity* m_dstEntity;
    
    public: 
        InteractionController(UiController& uiController);

        void StartInteraction(std::vector<Entity*> entities);
        void EndInteraction();
};
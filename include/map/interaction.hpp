#pragma once

#include <vector>

class Entity;
class UiController;

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
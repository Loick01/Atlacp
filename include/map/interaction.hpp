#pragma once

#include <vector>

class MapEntity;
class UiController;

class InteractionController
{
    private:
        UiController& m_uiController;
        MapEntity* m_srcEntity;
        MapEntity* m_dstEntity;
    
    public: 
        InteractionController(UiController& uiController);

        void StartInteraction(std::vector<MapEntity*> entities);
        void EndInteraction();
};
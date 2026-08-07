#pragma once

#include <vector>

class MapElement;
class MapEntity;
class UiController;

class InteractionController
{
    private:
        UiController& m_uiController;
        MapEntity* m_srcEntity;
        MapElement* m_dstElement;
    
    public: 
        InteractionController(UiController& uiController);

        void StartInteraction(std::vector<MapEntity*> entities, std::vector<MapElement*> elements); // NPCs are not in elements (even if they could because MapEntity inherits from MapElement)
        void ProcessInteraction();
        void EndInteraction();
};
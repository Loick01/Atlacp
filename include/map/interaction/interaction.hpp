#pragma once

#include <vector>

class MapElement;
class MapEntity;
class OrderController;

class InteractionController
{
    private:
        OrderController& m_orderController;
        MapEntity* m_srcEntity;
        MapElement* m_dstElement;

        void EndInteraction();
        
    public: 
        InteractionController(OrderController& orderController);

        void InitializeInteraction(const std::vector<MapEntity*>& entities, const std::vector<MapElement*>& elements); // NPCs are not in elements (even if they could because MapEntity inherits from MapElement)
        void StartInteraction();
        void ContinueInteraction();
};
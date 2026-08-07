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
        unsigned int m_nrOrder;
        unsigned int m_currentIndexOrder;
    
    public: 
        InteractionController(OrderController& orderController);

        void InitializeInteraction(std::vector<MapEntity*> entities, std::vector<MapElement*> elements); // NPCs are not in elements (even if they could because MapEntity inherits from MapElement)
        void StartInteraction();
        void NextOrder();
        void EndInteraction();
};
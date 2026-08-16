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

        bool HasOrder();
    
    public: 
        InteractionController(OrderController& orderController);

        void InitializeInteraction(const std::vector<MapEntity*>& entities, const std::vector<MapElement*>& elements); // NPCs are not in elements (even if they could because MapEntity inherits from MapElement)
        void StartInteraction();
        void UpdateOrder();
        void EndInteraction();
};
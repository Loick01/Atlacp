#pragma once

#include <vector>

struct DataMapElement;

class MapElement;
class MapEntity;
class OrderController;
class Tilemap;

class InteractionController
{
    private:
        std::vector<MapElement> m_interactions;
        OrderController& m_orderController;
        MapEntity* m_srcEntity;
        MapElement* m_dstElement;

        void EndInteraction();
        
    public: 
        InteractionController(OrderController& orderController);

        void SetInteractions(const std::vector<DataMapElement>& interactionsData, Tilemap& tilemap);

        void InitializeInteraction(const std::vector<MapEntity*>& entities);
        void StartInteraction();
        void ContinueInteraction();
};
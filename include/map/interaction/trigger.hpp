#pragma once

#include <vector>

struct DataMapElement;

class MapElement;
class MapEntity;
class OrderController;
class Tilemap;

class TriggerController // Should be merged with InteractionController ?
{
    private:
        std::vector<MapElement*> m_triggers;
        OrderController& m_orderController;
        MapEntity* m_triggeringEntity;

        void EndTrigger();

    public:
        TriggerController(OrderController& orderController);

        void SetTriggers(const std::vector<DataMapElement>& triggersData, Tilemap& tilemap); // Rename ? (Same than MapElementController::LoadElements())
    
        void LookForTrigger(MapEntity* entity);
        void ContinueTrigger();
};
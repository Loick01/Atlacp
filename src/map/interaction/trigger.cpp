#include "map/interaction/trigger.hpp"

#include "core/file/file.hpp" // DataMapElement
#include "map/map_element.hpp"
#include "map/map_entity.hpp"
#include "map/order/order_controller.hpp"
#include "tile/tilemap.hpp"

TriggerController::TriggerController(OrderController& orderController): 
    m_orderController(orderController), m_triggeringEntity(nullptr)
{
    
}

void TriggerController::SetTriggers(const std::vector<DataMapElement>& triggersData, Tilemap& tilemap)
{
    for (const DataMapElement& data : triggersData) {
        MapElement* e = new MapElement(tilemap);
        e->SetMapPosition(data.position);
        e->SetOrders(data.orders);
        m_triggers.push_back(e);
    }
}

void TriggerController::LookForTrigger(MapEntity* entity)
{
    for (MapElement* t : m_triggers) {
        if (t->GetMapPosition() == entity->GetMapPosition()) {
            entity->SetState(EntityState::Triggering);
            m_triggeringEntity = entity;
            m_orderController.AddOrders(t->GetOrders());
            m_orderController.NextOrder();
            break;
        }
    }
}

void TriggerController::ContinueTrigger()
{
    if (!m_orderController.NextOrder())
        EndTrigger();
}

void TriggerController::EndTrigger()
{
    m_triggeringEntity->SetState(EntityState::Free);
}
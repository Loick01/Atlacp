#include "map/interaction/trigger.hpp"

#include "core/file/file.hpp" // DataMapElement
#include "map/map_element.hpp"
#include "map/map_entity.hpp"
#include "map/order/order_controller.hpp"
#include "tile/tilemap.hpp"

TriggerController::TriggerController(OrderController& orderController): 
    m_orderController(orderController), m_triggeringEntity(nullptr)
{
    m_orderController.AddCallback([this](OrderExecutionEvent e){EndTrigger();});
}

void TriggerController::SetTriggers(const std::vector<DataMapElement>& triggersData, Tilemap& tilemap)
{
    // Remove (I will use smart ptr, I'm just not sure where to store them)
    for (MapElement* e : m_triggers)
        delete e; // Because I use new to create my MapElement (below)
    m_triggers.clear();
    
    for (const DataMapElement& data : triggersData) {
        MapElement* e = new MapElement(tilemap); // TODO
        e->SetMapPosition(data.position);
        e->SetOrders(data.orders);
        m_triggers.push_back(e);
    }
}

void TriggerController::LookForTrigger(MapEntity* entity)
{
    for (MapElement* t : m_triggers) {
        if (t->GetMapPosition() == entity->GetMapPosition()) {
            entity->SetInteractionState(EntityInteractionState::Triggering);
            m_triggeringEntity = entity;
            m_orderController.AddOrders(t->GetOrders());
            m_orderController.NextOrder();
            break;
        }
    }
}

void TriggerController::ContinueTrigger()
{
    m_orderController.NextOrder();
}

void TriggerController::EndTrigger()
{
    if (m_triggeringEntity != nullptr) {
        m_triggeringEntity->SetMovementState(EntityMovementState::Free);
        m_triggeringEntity->SetInteractionState(EntityInteractionState::None);
    }
}
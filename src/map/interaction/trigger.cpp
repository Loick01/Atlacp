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
    m_triggers.clear();
    m_triggers.reserve(triggersData.size());

    for (const DataMapElement& data : triggersData) {
        MapElement e(tilemap);
        e.SetMapPosition(data.position);
        e.SetOrders(data.orders);
        m_triggers.push_back(e);
    }
}

void TriggerController::LookForTrigger(MapEntity* entity)
{
    for (MapElement& trigger : m_triggers) {
        if (trigger.GetMapPosition() == entity->GetMapPosition()) {
            entity->SetInteractionState(EntityInteractionState::Triggering);
            m_triggeringEntity = entity;
            m_orderController.AddOrders(trigger.GetOrders());
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
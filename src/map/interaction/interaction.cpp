#include "map/interaction/interaction.hpp"

#include "map/order/order_controller.hpp"
#include "map/map_element.hpp"
#include "map/map_entity.hpp"

InteractionController::InteractionController(OrderController& orderController) :
    m_orderController(orderController), m_srcEntity(nullptr), m_dstElement(nullptr)
{}

void InteractionController::InitializeInteraction(const std::vector<MapEntity*>& entities, const std::vector<MapElement*>& elements)
{
    for (MapEntity* e : entities) { // Only the player will be able to start an interaction ? Or NPC will use Interaction system for cinematics ?
        // Only one interaction at a time
        if (e->GetState() == EntityState::Interacting) {
            m_srcEntity = e;
            break;
        }
    }
    const MapPosition target = m_srcEntity->GetTargetPosition();

    for (MapEntity* e : entities) {
        if (e->GetMapPosition() == target && e->GetState() == EntityState::Free) {
            m_dstElement = e;
            return;
        }
    }
    for (MapElement* e : elements) {
        if (e->GetMapPosition() == target) {
            m_dstElement = e;
            return;
        }
    }
}

void InteractionController::StartInteraction()
{
    if (m_dstElement == nullptr || m_dstElement->GetOrders().size() == 0) {
        // No target MapElement or MapEntity has been found OR no order to execute (only for NPC, if a MapElement is declared it must have at least one order)
        m_srcEntity->SetState(EntityState::Free);
        return;
    }
    
    m_dstElement->OnInteracting(m_srcEntity->GetCurrentMovement().GetOppositeDirection());
    // There is at least one order (m_dstElement->GetOrders().size() == 0)
    m_orderController.AddOrders(m_dstElement->GetOrders());
    m_orderController.NextOrder();
}

void InteractionController::ContinueInteraction()
{
    if (!m_orderController.NextOrder())
        EndInteraction();
}

void InteractionController::EndInteraction()
{
    m_srcEntity->SetState(EntityState::Free);
    m_dstElement->ReleaseInteracting();
    m_srcEntity = nullptr;
    m_dstElement = nullptr;
}
#include "map/interaction/interaction.hpp"

#include "map/interaction/order.hpp"
#include "map/map_element.hpp"
#include "map/map_entity.hpp"

InteractionController::InteractionController(OrderController& orderController) :
    m_orderController(orderController), m_srcEntity(nullptr), m_dstElement(nullptr)
{}

bool InteractionController::HasOrder()
{
    return m_currentIndexOrder < m_nrOrder;
}

void InteractionController::InitializeInteraction(std::vector<MapEntity*> entities, std::vector<MapElement*> elements)
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
    m_nrOrder = m_dstElement->GetOrders().size();
    m_currentIndexOrder = 0;

    // There is at least one order (m_dstElement->GetOrders().size() == 0)
    m_orderController.Execute(m_dstElement->GetOrders()[m_currentIndexOrder++]);
}

void InteractionController::UpdateOrder()
{
    const bool isOrderDone = m_orderController.Update(m_dstElement->GetOrders()[m_currentIndexOrder-1]);

    if (isOrderDone && !HasOrder()) {
        EndInteraction();
        return;
    }

    if (!isOrderDone)
        return;

    m_orderController.Stop(m_dstElement->GetOrders()[m_currentIndexOrder-1]);
    m_orderController.Execute(m_dstElement->GetOrders()[m_currentIndexOrder++]);
}

void InteractionController::EndInteraction()
{
    m_srcEntity->SetState(EntityState::Free);
    m_dstElement->ReleaseInteracting();
    // ExecuteOrder is necessarily called at least once before EndInteraction(), because m_dstElement->GetOrders().size() > 0 (InteractionController::StartInteraction())
    m_orderController.Stop(m_dstElement->GetOrders()[m_currentIndexOrder-1]);
    m_srcEntity = nullptr;
    m_dstElement = nullptr;
}
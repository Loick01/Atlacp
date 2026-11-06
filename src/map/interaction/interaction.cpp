#include "map/interaction/interaction.hpp"

#include "core/file/file.hpp" // DataMapElement
#include "map/order/order_controller.hpp"
#include "map/map_element.hpp"
#include "map/map_entity.hpp"
#include "tile/tilemap.hpp"

InteractionController::InteractionController(OrderController& orderController) :
    m_orderController(orderController), m_srcEntity(nullptr), m_dstElement(nullptr)
{
    m_orderController.AddCallback([this](OrderExecutionEvent e){EndInteraction();});
}

void InteractionController::SetInteractions(const std::vector<DataMapElement>& interactionsData, Tilemap& tilemap)
{
    m_interactions.clear();
    m_interactions.reserve(interactionsData.size());

    for (const DataMapElement& data : interactionsData) {
        MapElement e(tilemap);
        e.SetMapPosition(data.position);
        e.SetOrders(data.orders);
        m_interactions.push_back(e);
    }
}

void InteractionController::InitializeInteraction(const std::vector<MapEntity*>& entities)
{
    for (MapEntity* e : entities) { // TODO : Only the player will be able to start an interaction ? Or NPC will use Interaction system for cinematics ?
        // Only one interaction at a time
        if (e->GetInteractionState() == EntityInteractionState::Interacting) {
            m_srcEntity = e;
            break;
        }
    }
    const MapPosition target = m_srcEntity->GetTargetPosition();

    for (MapEntity* e : entities) {
        if (e->GetMapPosition() == target && e->GetMovementState() == EntityMovementState::Free && e->GetInteractionState() == EntityInteractionState::None) {
            m_dstElement = e;
            return;
        }
    }
    for (MapElement& e : m_interactions) {
        if (e.GetMapPosition() == target) {
            m_dstElement = &e;
            return;
        }
    }
}

void InteractionController::StartInteraction()
{
    if (m_dstElement == nullptr || m_dstElement->GetOrders().empty()) {
        // No target MapElement or MapEntity has been found OR no order to execute (only for NPC, if a MapElement is declared it must have at least one order)
        m_srcEntity->SetInteractionState(EntityInteractionState::None);
        return;
    }
    
    m_dstElement->OnInteracting(m_srcEntity->GetCurrentMovement().GetOppositeDirection());
    // There is at least one order (m_dstElement->GetOrders().empty())
    m_orderController.AddOrders(m_dstElement->GetOrders());
    m_orderController.NextOrder();
}

void InteractionController::ContinueInteraction()
{
    m_orderController.NextOrder();
}

void InteractionController::EndInteraction()
{
    if (m_srcEntity != nullptr && m_dstElement != nullptr) {
        m_srcEntity->ReleaseInteracting();
        m_dstElement->ReleaseInteracting();
        m_srcEntity = nullptr;
        m_dstElement = nullptr;
    }
}
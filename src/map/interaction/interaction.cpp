#include "map/interaction/interaction.hpp"

#include "map/interaction/order.hpp"
#include "map/map_element.hpp"
#include "map/map_entity.hpp"

InteractionController::InteractionController(OrderController& orderController) :
    m_orderController(orderController), m_srcEntity(nullptr), m_dstElement(nullptr)
{}

void InteractionController::StartInteraction(std::vector<MapEntity*> entities, std::vector<MapElement*> elements)
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

void InteractionController::ProcessInteraction()
{
    if (m_dstElement == nullptr) { // No target MapElement or MapEntity has been found
        m_srcEntity->SetState(EntityState::Free);
        return;
    }
    
    m_dstElement->OnInteracting(m_srcEntity->GetCurrentMovement().GetOppositeDirection());
    m_orderController.Execute(m_dstElement->GetOrders()[0]); // TODO : Not only the first Order
    // Do not interact with NPC, they don't have order for now
}

void InteractionController::EndInteraction()
{
    m_srcEntity->SetState(EntityState::Free);
    m_dstElement->ReleaseInteracting();
    m_srcEntity = nullptr;
    m_dstElement = nullptr;

    // Will be in OrderController
    // m_uiController.DeleteElement("frame");
    // SoundController::GetInstance().RequestChunk(BaseSfx::Close);
}
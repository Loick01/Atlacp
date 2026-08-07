#include "map/interaction.hpp"

#include "map/map_element.hpp"
#include "map/map_entity.hpp"
#include "sound/sound.hpp"
#include "ui/ui_controller.hpp"

InteractionController::InteractionController(UiController& uiController) :
    m_uiController(uiController), m_srcEntity(nullptr), m_dstElement(nullptr)
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

    // switch (orderType) {
    //     case OrderType::FrameText :
    //     case OrderType::DialogText :
    // }

    m_dstElement->OnInteracting(m_srcEntity->GetCurrentMovement().GetOppositeDirection());
    m_uiController.OpenDialogBox("Hello world ! This is an example of a long sentence to test how the text is wrapped by SDL_ttf..."); // TODO : Play order(s)
    SoundController::GetInstance().RequestChunk(BaseSfx::Open); // Should not be here ?
}

void InteractionController::EndInteraction()
{
    m_srcEntity->SetState(EntityState::Free);
    m_dstElement->ReleaseInteracting();
    m_srcEntity = nullptr;
    m_dstElement = nullptr;
    m_uiController.DeleteElement("frame"); // Should not be here ?
    SoundController::GetInstance().RequestChunk(BaseSfx::Close); // Should not be here ?
}
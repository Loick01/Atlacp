#include "interaction.hpp"

InteractionController::InteractionController(UiController& uiController) :
    m_uiController(uiController), m_srcEntity(nullptr), m_dstEntity(nullptr)
{}

void InteractionController::StartInteraction(std::vector<Entity*> entities)
{
    for (Entity* e : entities) { // Only the player will be able to start an interaction ? Or NPC will use Interaction system for cinematics ?
        // Only one interaction at a time
        if (e->GetState() == EntityState::Interacting) {
            m_srcEntity = e;
            break;
        }
    }

    const MapPosition target = m_srcEntity->GetTargetPosition();

    for (Entity* e : entities) {
        if (e->GetMapPosition() == target && e->GetState() == EntityState::Free) { // Will interact only with EntityState::Free
            m_dstEntity = e;
            break;
        }
    }
    if (m_dstEntity != nullptr) { // If an Entity has been found
        m_dstEntity->SetOrientation(m_srcEntity->GetCurrentMovement().GetOppositeDirection());
        m_dstEntity->SetState(EntityState::Interacting); // Targeted entity will not move
        m_uiController.OpenDialogBox("Hello world ! This is an example of a long sentence to test how the text is wrapped by SDL_ttf..."); // Will be removed
    } else { // Will be removed
        m_srcEntity->SetState(EntityState::Free);
    }
}

void InteractionController::EndInteraction()
{
    m_srcEntity->SetState(EntityState::Free);
    m_dstEntity->SetState(EntityState::Free); 
    m_srcEntity = nullptr;
    m_dstEntity = nullptr;
    m_uiController.DeleteElement("frame"); // Should not be here ?
}
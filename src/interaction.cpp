#include "interaction.hpp"

InteractionController::InteractionController() :
    m_uiController(nullptr), m_srcEntity(nullptr), m_dstEntity(nullptr)
{

}

void InteractionController::SetUiController(UiController* uiController)
{
    m_uiController = uiController;
}

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

    m_dstEntity->SetOrientation(m_srcEntity->GetCurrentMovement().GetOppositeDirection());
    m_dstEntity->SetState(EntityState::Interacting); // Targeted entity will not move
    // m_uiController->OpenDialogBox();
}
#include "battle/battle_controller.hpp"

#include "ui/component/ui_component_controller.hpp"
#include "ui/element/ui_controller.hpp"

void BattleController::OpenActionSelection()
{
    UiList* actionSelectionList = dynamic_cast<UiList*>(m_uiComponentController.GetComponent("actionSelectionList"));
    UiSelector* selector = dynamic_cast<UiSelector*>(m_uiComponentController.GetComponent("selector"));

    actionSelectionList->Open();
    selector->Open();
    selector->SetOptionKeys(actionSelectionList->GetItemsKey(), Axis::Height, Axis::Width, 1.f, -0.05f);
}

void BattleController::CloseActionSelection()
{
    m_uiComponentController.CloseComponent("selector");
    m_uiComponentController.CloseComponent("actionSelectionList");
}

void BattleController::OpenMoveSelection()
{
    const std::vector<MoveDefinition>& moves = m_currentActor->GetMoves();

    UiDynamicList* moveSelection = dynamic_cast<UiDynamicList*>(m_uiComponentController.GetComponent("moveSelection"));
    moveSelection->SetFirstItemParams(
        UiParams(m_uiController.GetResultFromPartialSize(PartialSize("frame", Axis::Width, 0.5f)), Axis::Width, // Scale
        Anchor::LeftIn, Anchor::TopIn, // Anchor
        m_uiController.GetResultFromPartialSize(PartialSize("frame", Axis::Width, 0.06f)), // Padding
        m_uiController.GetResultFromPartialSize(PartialSize("frame", Axis::Height, 0.25f))));
    moveSelection->SetNrItem(moves.size());
    moveSelection->Open();
    
    const std::vector<UiKey> keys = moveSelection->GetItemsKey();
    for (unsigned int i = 0 ; i < keys.size() ; i++) // keys.size = moves.size()
        m_uiController.EditText(keys[i], moves[i].name);

    UiSelector* selector = dynamic_cast<UiSelector*>(m_uiComponentController.GetComponent("selector"));
    selector->Open();
    selector->SetOptionKeys(keys, Axis::Height, Axis::Width, 1.f, -0.05f);
}

void BattleController::CloseMoveSelection()
{
    m_uiComponentController.CloseComponent("selector");
    m_uiComponentController.CloseComponent("moveSelection");
}

void BattleController::OpenActorSelection()
{
    UiSelector* selector = dynamic_cast<UiSelector*>(m_uiComponentController.GetComponent("selector"));
    selector->Open();
    std::vector<BattleActor*> actors = FilterActorsByBattleState(GetActorsInTeam(m_currentCommand.targetTeam), m_currentCommand.targetBattleState);
    std::vector<UiKey> keys;
    
    for (const BattleActor* actor : actors)
        keys.push_back(actor->GetSpritePath().id); // SpritePath.id is the parent key of each item in allyList/opponentList
    selector->SetOptionKeys(keys, Axis::Height, Axis::Width, 0.2f, 0.2f);

    UiFrameText* frameText = dynamic_cast<UiFrameText*>(m_uiComponentController.GetComponent("frameText"));
    frameText->Open();
    frameText->AddText({"Select a BattleActor"});
    frameText->NextText();
}

void BattleController::CloseActorSelection()
{
    m_uiComponentController.CloseComponent("frameText");
    m_uiComponentController.CloseComponent("selector");
}
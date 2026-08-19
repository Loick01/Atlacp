#include "map/order/order_controller.hpp"

#include "core/file/file.hpp"
#include "map/map_element_controller.hpp"
#include "map/npc.hpp" // NPC, MapBehaviour
#include "sound/sound.hpp"
#include "tile/tilemap.hpp"
#include "ui/component/ui_component_controller.hpp"
#include "ui/component/ui_dialog_box.hpp"
#include "ui/component/ui_frame_text.hpp"

OrderController::OrderController(FileReader& fileReader, MapElementController& mapElementController, Tilemap& tilemap, UiComponentController& uiComponentController) :
    m_fileReader(fileReader), m_mapElementController(mapElementController), m_tilemap(tilemap), m_uiComponentController(uiComponentController), m_hasCurrentOrder(false)
{}

void OrderController::Execute(Order& order)
{
    m_currentOrder = order; // The current order is used in Scene (not for all Order, currently only for NpcGoToOrder)

    std::visit(
        [this](const auto& o)
        {
            ExecuteOrder(o);
        },
        order
    );
}

bool OrderController::Update(const Order& order)
{
    return std::visit(
        [this](const auto& o)
        {
            return UpdateOrder(o);
        },
        order
    );
    throw std::runtime_error("No OrderController::UpdateOrder with this type of Order"); // ?
}

void OrderController::Stop(const Order& order)
{
    std::visit(
        [this](const auto& o)
        {
            StopOrder(o);
        },
        order
    );
}

void OrderController::ExecuteOrder(const FrameTextOrder& o)
{
    UiFrameText* boxText = m_uiComponentController.CreateFrameText("boxText", "frame_text.uif");
    boxText->Open();
    boxText->AddText(o.texts);
    boxText->NextText();

    SoundController::GetInstance().RequestChunk(BaseSfx::Open);
}

void OrderController::ExecuteOrder(const DialogTextOrder& o)
{
    UiDialogBox* dialogBox = m_uiComponentController.CreateDialogBox("dialogBox", "dialog_box.uif");
    dialogBox->Open();
    dialogBox->AddText(o.texts);
    dialogBox->NextText();
    dialogBox->SetFacePath(o.facePath);

    SoundController::GetInstance().RequestChunk(BaseSfx::Open);
}

void OrderController::ExecuteOrder(const NpcGoToOrder& o)
{
    NPC* npc = static_cast<NPC*>(m_mapElementController.GetMapEntityFromId(o.idNpc)); // Will be dynamic_cast, in case the order of the list changes (could be Player instead of NPC ?)
    npc->SetState(EntityState::Free); // Should be in SetGoToBehaviour() ? 
    // It needs to be EntityState::Free otherwise it would prevent the NPC from moving, but Player can still interact with NPC ? Do I need a new state ?
    npc->SetGoToBehaviour(m_tilemap, o.targetPosition);
    MapGoToBehaviour* goTo = dynamic_cast<MapGoToBehaviour*>(npc->GetMapBehaviour());
    goTo->AddCallback([this](UselessEvent e){NextOrder();});
    // If the NPC is already at its target, the path is empty. In this scenario, the behaviour will never Notify() and Order execution will be blocked
    if (goTo->IsDone()) NextOrder();
}

void OrderController::ExecuteOrder(const PlayCinematicOrder& o)
{
    std::vector<Order> orders = m_fileReader.ReadCinematicFile(o.cinematicFilepath);
    AddOrders(orders);
    NextOrder(); // Because UpdateOrder(PlayCinematicOrder) returns true, NextOrder() will directly execute the first order in cinematic file 
}

bool OrderController::UpdateOrder(const FrameTextOrder& o)
{
    UiFrameText* boxText = dynamic_cast<UiFrameText*>(m_uiComponentController.GetComponent("boxText"));
    SoundController::GetInstance().RequestChunk(BaseSfx::Open);
    return !boxText->NextText();
}

bool OrderController::UpdateOrder(const DialogTextOrder& o)
{
    UiDialogBox* dialogBox = dynamic_cast<UiDialogBox*>(m_uiComponentController.GetComponent("dialogBox"));
    SoundController::GetInstance().RequestChunk(BaseSfx::Open);
    // I suppose the faceset will never be modified
    return !dialogBox->NextText();
}

bool OrderController::UpdateOrder(const NpcGoToOrder& o)
{
    NPC* npc = static_cast<NPC*>(m_mapElementController.GetMapEntityFromId(o.idNpc)); // Will be dynamic_cast, in case the order of the list changes
    const MapGoToBehaviour* goTo = dynamic_cast<const MapGoToBehaviour*>(npc->GetMapBehaviour());
    return goTo->IsDone();
}

bool OrderController::UpdateOrder(const PlayCinematicOrder& o)
{
    return true; // Do nothing else
}

void OrderController::StopOrder(const FrameTextOrder& o)
{
    m_uiComponentController.CloseComponent("boxText");
    m_uiComponentController.DeleteComponent("boxText");
    SoundController::GetInstance().RequestChunk(BaseSfx::Close);
}

void OrderController::StopOrder(const NpcGoToOrder& o)
{
    NPC* npc = static_cast<NPC*>(m_mapElementController.GetMapEntityFromId(o.idNpc)); // Will be dynamic_cast, in case the order of the list changes
    npc->SetRandomBehaviour();
    // The NPC has necessarily EntityState::Free when he reached his target position, so I don't think SetState(Free) is necessary. 
    // npc->SetState(EntityState::Free); // Should be in SetRandomBehaviour() ?
}

void OrderController::StopOrder(const DialogTextOrder& o)
{
    m_uiComponentController.CloseComponent("dialogBox");
    m_uiComponentController.DeleteComponent("dialogBox");
    SoundController::GetInstance().RequestChunk(BaseSfx::Close);
}

void OrderController::StopOrder(const PlayCinematicOrder& o)
{
    return; // ExecuteOrder(PlayCinematic) just fills the Order queue, so nothing to stop or delete here
}

void OrderController::AddOrders(const std::vector<Order>& orders)
{
    for (const Order& o : orders)
        m_orders.push(o);
}

bool OrderController::NextOrder()
{
    if (m_hasCurrentOrder) {
        const bool isOrderDone = Update(m_currentOrder);
        if (isOrderDone && m_orders.empty()) {
            Stop(m_currentOrder);
            m_hasCurrentOrder = false;
            return false;
        }

        if (!isOrderDone)
            return true;
        
        Stop(m_currentOrder);
    }

    if (m_orders.empty()) // Remove
        throw std::runtime_error("OrderController::m_orders should not be empty when reaching here");
    
    m_currentOrder = m_orders.front();
    m_hasCurrentOrder = true;
    m_orders.pop();
    Execute(m_currentOrder);
    return true;
}
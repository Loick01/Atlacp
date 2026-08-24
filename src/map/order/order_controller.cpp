#include "map/order/order_controller.hpp"

#include "core/camera.hpp"
#include "core/file/file.hpp"
#include "map/map_element_controller.hpp"
#include "map/npc.hpp" // NPC, MapBehaviour
#include "sound/sound.hpp"
#include "tile/tilemap.hpp"
#include "ui/component/ui_component_controller.hpp"
#include "ui/component/ui_dialog_box.hpp"
#include "ui/component/ui_frame_text.hpp"

OrderController::OrderController(Camera& camera, FileReader& fileReader, MapElementController& mapElementController,
Tilemap& tilemap, UiComponentController& uiComponentController) :
    m_camera(camera), m_fileReader(fileReader), m_mapElementController(mapElementController), 
    m_tilemap(tilemap), m_uiComponentController(uiComponentController), m_hasCurrentOrder(false)
{}

void OrderController::Execute(Order& order)
{
    m_currentOrder = order; // The current order is used in Scene (not for all Order, currently only for NpcGoToOrder)

    return std::visit( // Use return (even if void) to exit the function before reaching throw runtime_error
        [this](const auto& o)
        {
            ExecuteOrder(o);
        },
        order
    );
    throw std::runtime_error("No OrderController::ExecuteOrder with this type of Order"); // Every type in variant Order must have a ExecuteOrder 
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
    UpdateOrder(order); // Call UpdateOrder(const Order&)
}

void OrderController::Stop(const Order& order)
{
    return std::visit( // Use return (even if void) to exit the function before calling StopOrder(const Order&)
        [this](const auto& o)
        {
            StopOrder(o);
        },
        order
    );
    StopOrder(order); // Call StopOrder(const Order&)
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

void OrderController::ExecuteOrder(const NpcFollowOrder& o)
{
    NPC* npc = static_cast<NPC*>(m_mapElementController.GetMapEntityFromId(o.idNpc)); // Will be dynamic_cast, in case the order of the list changes (could be Player instead of NPC ?)
    MapEntity* trackedEntity = m_mapElementController.GetMapEntityFromId(o.idTrackedEntity);
    npc->SetState(EntityState::Free); // Useless ?
    npc->SetFollowBehaviour(trackedEntity);
    NextOrder();
}

void OrderController::ExecuteOrder(const NpcIdleOrder& o)
{
    NPC* npc = static_cast<NPC*>(m_mapElementController.GetMapEntityFromId(o.idNpc)); // Will be dynamic_cast, in case the order of the list changes (could be Player instead of NPC ?)
    npc->SetState(EntityState::Free); // Useless ?
    npc->SetIdleBehaviour(o.direction);
    NextOrder();
}

void OrderController::ExecuteOrder(const PlayCinematicOrder& o)
{
    std::vector<Order> orders = m_fileReader.ReadCinematicFile(o.cinematicFilepath);
    AddOrders(orders);
    NextOrder(); 
    // Because UpdateOrder(PlayCinematicOrder) does not exist, it will use UpdateOrder(Order) that returns true
    // So NextOrder() will directly execute the first order in cinematic file 
}

void OrderController::ExecuteOrder(const CameraSlideToOrder& o)
{
    m_camera.StartSlidingTo(o.endPosition);
    m_camera.AddCallback([this](UselessEvent e){NextOrder();});
    // if (m_camera.IsDone()) NextOrder(); // I think it's useless, even if the camera is already at its targeted position, Notify() will be called anyway // Remove
}

bool OrderController::UpdateOrder(const Order& o)
{
    return true; // Do nothing else
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

bool OrderController::UpdateOrder(const CameraSlideToOrder& o)
{
    return m_camera.GetAnimState() == CameraAnimState::Done;
}

void OrderController::StopOrder(const Order& o)
{
    return; // When there is nothing to stop or delete
}

void OrderController::StopOrder(const FrameTextOrder& o)
{
    m_uiComponentController.CloseComponent("boxText");
    m_uiComponentController.DeleteComponent("boxText");
    SoundController::GetInstance().RequestChunk(BaseSfx::Close);
}

void OrderController::StopOrder(const DialogTextOrder& o)
{
    m_uiComponentController.CloseComponent("dialogBox");
    m_uiComponentController.DeleteComponent("dialogBox");
    SoundController::GetInstance().RequestChunk(BaseSfx::Close);
}

void OrderController::StopOrder(const NpcGoToOrder& o)
{
    NPC* npc = static_cast<NPC*>(m_mapElementController.GetMapEntityFromId(o.idNpc)); // Will be dynamic_cast, in case the order of the list changes
    npc->SetRandomBehaviour();
    // The NPC has necessarily EntityState::Free when he reached his target position, so I don't think SetState(Free) is necessary. 
    // npc->SetState(EntityState::Free); // Should be in SetRandomBehaviour() ?
}

void OrderController::StopOrder(const CameraSlideToOrder& o) // Could be removed and use only CameraAnimState::Free/Sliding ?
{
    m_camera.SetAnimState(CameraAnimState::Free);
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
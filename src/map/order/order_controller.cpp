#include "map/order/order_controller.hpp"

#include "core/camera.hpp"
#include "core/file/file.hpp"
#include "core/logger.hpp"
#include "core/time.hpp"
#include "map/map_entity_controller.hpp"
#include "map/npc.hpp" // NPC, MapBehaviour
#include "sound/sound.hpp"
#include "tile/tilemap.hpp"
#include "ui/component/ui_component_controller.hpp"
#include "ui/component/ui_dialog_box.hpp"
#include "ui/component/ui_frame_text.hpp"
#include "window/boxing.hpp"

OrderController::OrderController(Boxing& boxing, Camera& camera, FileReader& fileReader, MapEntityController& mapElementController,
Tilemap& tilemap, Time& time, UiComponentController& uiComponentController) :
    m_boxing(boxing), m_camera(camera), m_fileReader(fileReader), m_mapEntityController(mapElementController), 
    m_tilemap(tilemap), m_time(time), m_uiComponentController(uiComponentController), m_hasCurrentOrder(false)
{}

bool OrderController::HasNoOrders() const
{
    return m_orders.empty();
}

bool OrderController::GetHasCurrentOrder() const
{
    return m_hasCurrentOrder;
}

void OrderController::Execute(Order& order)
{
    m_currentOrder = order; // The current order is used in Scene (not for all Order, currently only for NpcGoToOrder)

    return std::visit( // Use return (even if void) to exit the function before reaching the Log
        [this](const auto& o)
        {
            ExecuteOrder(o);
        },
        order
    );
    Logger::GetInstance().AddLog(Level::L4, "No OrderController::ExecuteOrder with this type of Order"); // Every type in variant Order must have a ExecuteOrder // TODO : Log
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

    SoundController::GetInstance().RequestChunk(std::string(BaseSfx::Open));
}

void OrderController::ExecuteOrder(const DialogTextOrder& o)
{
    UiDialogBox* dialogBox = m_uiComponentController.CreateDialogBox("dialogBox", "dialog_box.uif");
    dialogBox->Open();
    dialogBox->AddText(o.texts);
    dialogBox->NextText();
    dialogBox->SetFacePath(o.facePath);

    SoundController::GetInstance().RequestChunk(std::string(BaseSfx::Open));
}

void OrderController::ExecuteOrder(const NpcGoToOrder& o)
{
    NPC* npc = static_cast<NPC*>(m_mapEntityController.GetMapEntityFromId(o.idNpc)); // TODO : Will be dynamic_cast, in case the order of the list changes (could be Player instead of NPC ?)
    npc->SetMovementState(EntityMovementState::Free); // TODO : Should be in SetGoToBehaviour() ? 
    // TODO : It needs to be EntityMovementState::Free otherwise it would prevent the NPC from moving, but Player can still interact with NPC ? Do I need a new state ?
    npc->SetGoToBehaviour(m_tilemap, o.targetPosition);
    
    // SetGoToBehaviour() can fail (when there is no path from start to end position)
    MapGoToBehaviour* goTo = dynamic_cast<MapGoToBehaviour*>(npc->GetMapBehaviour());
    if (goTo != nullptr && goTo->IsInit()) {
        goTo->AddCallback([this](UselessEvent e){NextOrder();});
        // If the NPC is already at its target, the path is empty. In this scenario, the behaviour will never Notify() and Order execution will be blocked
        if (goTo->IsDone()) NextOrder(); // That's why I need this line
    } else {
        Order order = NpcIdleOrder{Direction::Down, o.idNpc};
        Execute(order);
    }
}

void OrderController::ExecuteOrder(const NpcFollowOrder& o)
{
    NPC* npc = static_cast<NPC*>(m_mapEntityController.GetMapEntityFromId(o.idNpc)); // TODO : Will be dynamic_cast, in case the order of the list changes (could be Player instead of NPC ?)
    MapEntity* trackedEntity = m_mapEntityController.GetMapEntityFromId(o.idTrackedEntity);
    npc->SetMovementState(EntityMovementState::Free); // TODO : Useless ?
    npc->SetFollowBehaviour(trackedEntity);
    if (npc->GetMapBehaviour()->IsInit()) {
        NextOrder();
    } else {
        Order order = NpcIdleOrder{Direction::Down, o.idNpc};
        Execute(order);
    }
}

void OrderController::ExecuteOrder(const NpcIdleOrder& o)
{
    NPC* npc = static_cast<NPC*>(m_mapEntityController.GetMapEntityFromId(o.idNpc)); // TODO : Will be dynamic_cast, in case the order of the list changes (could be Player instead of NPC ?)
    npc->SetMovementState(EntityMovementState::Free); // TODO : Useless ?
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

void OrderController::ExecuteOrder(const CameraSlideToPositionOrder& o)
{
    m_camera.StartSlidingTo(o.endPosition);
    m_camera.AddCallback([this](UselessEvent e){NextOrder();});
    // if (m_camera.IsDone()) NextOrder(); // TODO : Remove, because I think it's useless, even if the camera is already at its targeted position, Notify() will be called anyway
}

void OrderController::ExecuteOrder(const CameraSlideToEntityOrder& o)
{
    MapEntity* entity = m_mapEntityController.GetMapEntityFromId(o.idEntity); // Could be Player (id = 0) or NPC
    m_camera.StartSlidingTo(entity->GetScenePosition() + entity->GetDisplayOffset());
    m_camera.AddCallback([this](UselessEvent e){NextOrder();});
}

void OrderController::ExecuteOrder(const CameraAnchorEntityOrder& o)
{
    MapEntity* entity = m_mapEntityController.GetMapEntityFromId(o.idEntity); // Could be Player (id = 0) or NPC
    m_camera.SetAnchoredEntity(entity);
    NextOrder();
}

void OrderController::ExecuteOrder(const EntityOrientationOrder& o)
{
    MapEntity* entity = m_mapEntityController.GetMapEntityFromId(o.idEntity); // Could be Player (id = 0) or NPC
    entity->SetOrientation(o.direction);
    NextOrder();
}

void OrderController::ExecuteOrder(const EntityCreateOrder& o)
{
    m_mapEntityController.CreateEntity(o.entityFilepath, o.idEntity, o.spawnPosition);
    NextOrder();
}

void OrderController::ExecuteOrder(const EntityDeleteOrder& o)
{
    m_mapEntityController.DeleteEntity(o.idEntity);
    NextOrder();
}

void OrderController::ExecuteOrder(const TimeDelayOrder& o)
{
    m_time.SetDelay(o.delay);
    m_time.AddCallback([this](UselessEvent e){NextOrder();});
}

void OrderController::ExecuteOrder(const LoadMapOrder& o)
{
    m_tilemap.LoadMapByIndex(o.mapIndex);

    Player* player = static_cast<Player*>(m_mapEntityController.GetMapEntityFromId(0));

    switch (o.reposition) {
        case PlayerReposition::MapSpawn :
            player->SetMapPosition(m_tilemap.GetSpawnPosition());
            break;
        case PlayerReposition::Projection :
            player->SetMapPosition(m_tilemap.GetProjectedPosition(player->GetMapPosition()));
            m_tilemap.SetCurrentBound(MapBound::Inside);
            break;
        default :
            Logger::GetInstance().AddLog(Level::L4, "Unhandled PlayerReposition used in ExecuteOrder(LoadMapOrder)"); // TODO : Log
            break;
    }
    
    const ScenePosition newPosition = player->GetMapPosition().ToScenePosition(m_tilemap.GetTileSize());
    player->UpdateDrawingPosition(newPosition);
    // TODO : Reset(); ? Will also reset the sprite animation when loading a new map, maybe I don't want that
    
    NextOrder();
}

void OrderController::ExecuteOrder(const BoxingAnimationOrder& o)
{
    m_boxing.StartAnimation(o.speed);
    m_boxing.AddCallback([this](UselessEvent e){NextOrder();});
}

bool OrderController::UpdateOrder(const Order& o)
{
    return true; // Do nothing else
}

bool OrderController::UpdateOrder(const FrameTextOrder& o)
{
    UiFrameText* boxText = dynamic_cast<UiFrameText*>(m_uiComponentController.GetComponent("boxText"));
    SoundController::GetInstance().RequestChunk(std::string(BaseSfx::Open));
    return !boxText->NextText();
}

bool OrderController::UpdateOrder(const DialogTextOrder& o)
{
    UiDialogBox* dialogBox = dynamic_cast<UiDialogBox*>(m_uiComponentController.GetComponent("dialogBox"));
    SoundController::GetInstance().RequestChunk(std::string(BaseSfx::Open));
    // I suppose the faceset will never be modified
    return !dialogBox->NextText();
}

bool OrderController::UpdateOrder(const NpcGoToOrder& o)
{
    NPC* npc = static_cast<NPC*>(m_mapEntityController.GetMapEntityFromId(o.idNpc)); // TODO : Will be dynamic_cast, in case the order of the list changes
    const MapGoToBehaviour* goTo = dynamic_cast<const MapGoToBehaviour*>(npc->GetMapBehaviour());
    return goTo->IsDone();
}

bool OrderController::UpdateOrder(const CameraSlideToPositionOrder& o)
{
    return m_camera.GetCameraState() == CameraState::DoneSliding;
}

bool OrderController::UpdateOrder(const CameraSlideToEntityOrder& o)
{
    return m_camera.GetCameraState() == CameraState::DoneSliding;
}

bool OrderController::UpdateOrder(const TimeDelayOrder& o)
{
    return m_time.GetDelay() == 0.f;
}

bool OrderController::UpdateOrder(const BoxingAnimationOrder& o)
{
    return m_boxing.GetAnimationState() == BoxingState::Fixed;
}

void OrderController::StopOrder(const Order& o)
{
    return; // When there is nothing to stop or delete
}

void OrderController::StopOrder(const FrameTextOrder& o)
{
    m_uiComponentController.CloseComponent("boxText");
    m_uiComponentController.DeleteComponent("boxText");
    SoundController::GetInstance().RequestChunk(std::string(BaseSfx::Close));
}

void OrderController::StopOrder(const DialogTextOrder& o)
{
    m_uiComponentController.CloseComponent("dialogBox");
    m_uiComponentController.DeleteComponent("dialogBox");
    SoundController::GetInstance().RequestChunk(std::string(BaseSfx::Close));
}

void OrderController::StopOrder(const NpcGoToOrder& o)
{
    NPC* npc = static_cast<NPC*>(m_mapEntityController.GetMapEntityFromId(o.idNpc)); // TODO : Will be dynamic_cast, in case the order of the list changes
    npc->SetRandomBehaviour();
    // The NPC has necessarily EntityMovementState::Free when he reached his target position, so I don't think SetMovementState(Free) is necessary. 
    // npc->SetMovementState(EntityMovementState::Free); // TODO : Should be in SetRandomBehaviour() ?
}

void OrderController::StopOrder(const CameraSlideToPositionOrder& o)
{
    m_camera.SetCameraState(CameraState::Anchored); // TODO : Anchored ? Maybe I should set it to CameraState::Free and then use a CameraStateToAnchoredOrder(idEntity) ?
    m_camera.RemoveLastCallback();
}

void OrderController::StopOrder(const CameraSlideToEntityOrder& o)
{
    m_camera.SetCameraState(CameraState::Anchored); // TODO : Anchored ? Maybe I should set it to CameraState::Free and then use a CameraStateToAnchoredOrder(idEntity) ?
    m_camera.RemoveLastCallback();
}

void OrderController::StopOrder(const TimeDelayOrder& o)
{
    // Time::m_delay is already reset at 0.f in Time::Update()
    m_time.RemoveLastCallback();
}

void OrderController::StopOrder(const BoxingAnimationOrder& o)
{
    m_boxing.RemoveLastCallback();
}

void OrderController::AddOrders(const std::vector<Order>& orders)
{
    for (const Order& o : orders)
        m_orders.push(o);
}

void OrderController::NextOrder()
{
    if (m_hasCurrentOrder) {
        const bool isOrderDone = Update(m_currentOrder);
        if (isOrderDone && HasNoOrders()) {
            Stop(m_currentOrder);
            m_hasCurrentOrder = false;
            Notify(OrderExecutionEvent::End);
            return;
        }

        if (!isOrderDone)
            return;
        
        Stop(m_currentOrder);
    }

    if (HasNoOrders()) {
        Logger::GetInstance().AddLog(Level::L4, "OrderController::m_orders should not be empty when reaching here"); // TODO : Log
        return;
    }
    
    m_currentOrder = m_orders.front();
    m_hasCurrentOrder = true;
    m_orders.pop();
    Execute(m_currentOrder);
}
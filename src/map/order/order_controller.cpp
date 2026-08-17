#include "map/order/order_controller.hpp"

#include "sound/sound.hpp"
#include "ui/component/ui_component_controller.hpp"
#include "ui/component/ui_dialog_box.hpp"
#include "ui/component/ui_frame_text.hpp"

OrderController::OrderController(UiComponentController& uiComponentController) :
    m_uiComponentController(uiComponentController), m_hasCurrentOrder(false)
{}

const Order& OrderController::GetCurrentOrder() const
{
    return m_currentOrder;
}

Order& OrderController::GetCurrentOrder()
{
    return m_currentOrder;
}

std::string OrderController::GetStringDescription(const Order& order)
{
    return std::visit(
        [](const auto& o)
        {
            return GetStringFromOrder(o);
        },
        order
    );
    throw std::runtime_error("No OrderController::GetStringFromOrder with this type of Order"); // ?
}

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

std::string OrderController::GetStringFromOrder(const FrameTextOrder& o)
{
    std::string res = "frame_text " + std::to_string(o.texts.size()) + " ";
    for (const std::string& s : o.texts)
        res += s + ";";
    return res;
}

std::string OrderController::GetStringFromOrder(const DialogTextOrder& o)
{
    std::string res = "dialog_text " + std::to_string(o.texts.size()) + " " + o.facePath + " ";
    for (const std::string& s : o.texts)
        res += s + ";";
    return res;
}

std::string OrderController::GetStringFromOrder(const NpcGoToOrder& o)
{
    std::string res = "npc_goto " + std::to_string(o.targetPosition.x) + " " + std::to_string(o.targetPosition.y) + " " + std::to_string(o.idNpc);
    return res;
}

std::string OrderController::GetStringFromOrder(const PlayCinematicOrder& o)
{
    std::string res = "play_cinematic " + o.cinematicFilepath;
    return res;
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
    // Because OrderController can't have MapElementController&, I have no choice to use 
    // my Notifier class when it comes to order that modify MapEntity states
    Notify(OrderEvent::StartNpcGoTo);
}

void OrderController::ExecuteOrder(const PlayCinematicOrder& o)
{
    // TODO
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
    // Could the facepath be modified ?
    
    return !dialogBox->NextText();
}

bool OrderController::UpdateOrder(const NpcGoToOrder& o)
{
    Notify(OrderEvent::QueryGoToIsDone); // GameMapScene will set NpcGoToOrder::isDone
    // Thus InteractionController will continue with the next order only if the current GoToBehaviour is done (NPC has reached its target)
    return o.isDone;
}

bool OrderController::UpdateOrder(const PlayCinematicOrder& o)
{
    return true;
}

void OrderController::StopOrder(const FrameTextOrder& o)
{
    m_uiComponentController.CloseComponent("boxText");
    m_uiComponentController.DeleteComponent("boxText");
    SoundController::GetInstance().RequestChunk(BaseSfx::Close);
}

void OrderController::StopOrder(const NpcGoToOrder& o)
{
    Notify(OrderEvent::StopNpcGoTo);
}

void OrderController::StopOrder(const DialogTextOrder& o)
{
    m_uiComponentController.CloseComponent("dialogBox");
    m_uiComponentController.DeleteComponent("dialogBox");
    SoundController::GetInstance().RequestChunk(BaseSfx::Close);
}

void OrderController::StopOrder(const PlayCinematicOrder& o)
{
    // TODO
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
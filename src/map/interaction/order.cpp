#include "map/interaction/order.hpp"

#include "sound/sound.hpp"
#include "ui/ui_controller.hpp"

OrderController::OrderController(UiController& uiController) :
    m_uiController(uiController)
{}

void OrderController::Execute(const Order& order)
{
    std::visit(
        [this](const auto& o)
        {
            ExecuteOrder(o);
        },
        order
    );
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
    m_uiController.OpenDialogBox(o.text);
    SoundController::GetInstance().RequestChunk(BaseSfx::Open);
}

void OrderController::ExecuteOrder(const DialogTextOrder& o)
{
    // TODO
}

void OrderController::StopOrder(const FrameTextOrder& o)
{
    m_uiController.DeleteElement("frame"); // TODO : Do not use "frame" directly
    SoundController::GetInstance().RequestChunk(BaseSfx::Close);
}

void OrderController::StopOrder(const DialogTextOrder& o)
{
    // TODO
}
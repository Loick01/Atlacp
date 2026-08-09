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
    // TODO
}

void OrderController::ExecuteOrder(const DialogTextOrder& o)
{
    m_uiController.OpenDialogBox(o.text, o.facePath);
    SoundController::GetInstance().RequestChunk(BaseSfx::Open);
}

void OrderController::StopOrder(const FrameTextOrder& o)
{
    // TODO
}

void OrderController::StopOrder(const DialogTextOrder& o)
{
    m_uiController.CloseDialogBox(); // TODO : DialogBox should be a UiComponent
    SoundController::GetInstance().RequestChunk(BaseSfx::Close);
}
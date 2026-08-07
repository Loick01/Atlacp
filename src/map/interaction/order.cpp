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

void OrderController::ExecuteOrder(const FrameTextOrder& o)
{
    m_uiController.OpenDialogBox(o.text);
    SoundController::GetInstance().RequestChunk(BaseSfx::Open);
}

void OrderController::ExecuteOrder(const DialogTextOrder& o)
{
    m_uiController.OpenDialogBox(o.text);
    SoundController::GetInstance().RequestChunk(BaseSfx::Open);
}
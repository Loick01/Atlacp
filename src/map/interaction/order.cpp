#include "map/interaction/order.hpp"

#include "sound/sound.hpp"
#include "ui/component/ui_component_controller.hpp"
#include "ui/component/ui_dialog_box.hpp"

OrderController::OrderController(UiComponentController& uiComponentController) :
    m_uiComponentController(uiComponentController)
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
    UiDialogBox* dialogBox = m_uiComponentController.CreateDialogBox("dialogBox", "dialog_box.uif");
    dialogBox->Open();
    dialogBox->SetText(o.text);
    dialogBox->SetFacePath(o.facePath);

    SoundController::GetInstance().RequestChunk(BaseSfx::Open);
}

void OrderController::StopOrder(const FrameTextOrder& o)
{
    // TODO
}

void OrderController::StopOrder(const DialogTextOrder& o)
{
    m_uiComponentController.CloseComponent("dialogBox");
    m_uiComponentController.DeleteComponent("dialogBox");
    SoundController::GetInstance().RequestChunk(BaseSfx::Close);
}
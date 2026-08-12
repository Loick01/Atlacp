#include "map/interaction/order.hpp"

#include "sound/sound.hpp"
#include "ui/component/ui_component_controller.hpp"
#include "ui/component/ui_dialog_box.hpp"
#include "ui/component/ui_frame_text.hpp"

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
#pragma once

#include <string>
#include <variant>

struct FrameTextOrder {
    std::string text;
};

struct DialogTextOrder {
    std::string text;
    std::string facePath;
};

// struct AddInventoryOrder {};
// struct StartCinematicOrder {};

using Order = std::variant<
    FrameTextOrder,
    DialogTextOrder
>;

class UiComponentController;
class UiDialogBox;
class UiFrameText;

class OrderController
{
    private:
        UiComponentController& m_uiComponentController;

        void ExecuteOrder(const FrameTextOrder& o);
        void ExecuteOrder(const DialogTextOrder& o);

        void StopOrder(const FrameTextOrder& o);
        void StopOrder(const DialogTextOrder& o);

    public:
        OrderController(UiComponentController& uiComponentController);

        void Execute(const Order& order);
        void Stop(const Order& order); // Rename ?
};
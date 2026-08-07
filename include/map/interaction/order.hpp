#pragma once

#include <string>
#include <variant>

struct FrameTextOrder {
    std::string text;
};

struct DialogTextOrder {
    std::string text;
    std::string facesetPath;
};

// struct AddInventoryOrder {};
// struct StartCinematicOrder {};

using Order = std::variant<
    FrameTextOrder,
    DialogTextOrder
>;

class UiController; 

class OrderController // Should be in order_controller.hpp ?
{
    private:
        UiController& m_uiController;

        void ExecuteOrder(const FrameTextOrder& o);
        void ExecuteOrder(const DialogTextOrder& o);

    public:
        OrderController(UiController& uiController);

        void Execute(const Order& order);
};
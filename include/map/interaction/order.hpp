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

class OrderController // Should be in order_controller.hpp ?
{
    private:
    
    public:
        OrderController();
};
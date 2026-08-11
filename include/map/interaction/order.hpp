#pragma once

#include <string>
#include <variant>
#include <vector>

struct FrameTextOrder {
    std::vector<std::string> texts;
};

struct DialogTextOrder { // TODO : Multiple text
    std::vector<std::string> texts;
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

        bool UpdateOrder(const FrameTextOrder& o);
        bool UpdateOrder(const DialogTextOrder& o);

        void StopOrder(const FrameTextOrder& o);
        void StopOrder(const DialogTextOrder& o);

    public:
        OrderController(UiComponentController& uiComponentController);

        void Execute(const Order& order);
        bool Update(const Order& order);
        void Stop(const Order& order); // Rename ?
};
#pragma once

#include <string>
#include <variant>
#include <vector>

#include "core/notifier.hpp"
#include "map/map_types.hpp" // MapPosition

enum class OrderEvent
{
    NpcGoTo // Should be MapEntityGoTo (NPC + Player) ?
};

struct FrameTextOrder {
    std::vector<std::string> texts;
};

// UiDialogBox inherits from UiFrameText
struct DialogTextOrder { 
    std::vector<std::string> texts;
    std::string facePath;
};

struct NpcGoToOrder { // Should be the same order for Player (for now only NPC)
    MapPosition targetPosition;
    unsigned int idNpc;
};

// TODO :
// struct AddInventoryOrder {};
// struct PlayCinematicOrder {};
// Orders about camera

using Order = std::variant<
    FrameTextOrder,
    DialogTextOrder,
    NpcGoToOrder
>;

class UiComponentController;
class UiDialogBox;
class UiFrameText;

class OrderController : public Notifier<OrderEvent>
{
    private:
        UiComponentController& m_uiComponentController;
        Order m_currentOrder;

        void ExecuteOrder(const FrameTextOrder& o);
        void ExecuteOrder(const DialogTextOrder& o);
        void ExecuteOrder(const NpcGoToOrder& o);

        bool UpdateOrder(const FrameTextOrder& o);
        bool UpdateOrder(const DialogTextOrder& o);
        bool UpdateOrder(const NpcGoToOrder& o);

        void StopOrder(const FrameTextOrder& o);
        void StopOrder(const DialogTextOrder& o);
        void StopOrder(const NpcGoToOrder& o);

    public:
        OrderController(UiComponentController& uiComponentController);

        Order GetCurrentOrder() const;
        
        void Execute(const Order& order);
        bool Update(const Order& order);
        void Stop(const Order& order); // Rename ?
};
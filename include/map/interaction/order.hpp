#pragma once

#include <string>
#include <variant>
#include <vector>

#include "core/notifier.hpp"
#include "map/map_types.hpp" // MapPosition

enum class OrderEvent
{
    StartNpcGoTo, QueryGoToIsDone, StopNpcGoTo // Should be MapEntityGoTo (NPC + Player) ? Rename QueryGoToIsDone ?
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
    bool isDone;
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
        Order* m_currentOrder;

        static std::string GetStringFromOrder(const FrameTextOrder& o);
        static std::string GetStringFromOrder(const DialogTextOrder& o);
        static std::string GetStringFromOrder(const NpcGoToOrder& o);

        void ExecuteOrder(const FrameTextOrder& o);
        void ExecuteOrder(const DialogTextOrder& o);
        void ExecuteOrder(const NpcGoToOrder& o);

        bool UpdateOrder(const FrameTextOrder& o); // Rename
        bool UpdateOrder(const DialogTextOrder& o); // Rename
        bool UpdateOrder(const NpcGoToOrder& o); // Rename

        void StopOrder(const FrameTextOrder& o);
        void StopOrder(const DialogTextOrder& o);
        void StopOrder(const NpcGoToOrder& o);

    public:
        OrderController(UiComponentController& uiComponentController);

        const Order& GetCurrentOrder() const;
        Order& GetCurrentOrder();
        
        static std::string GetStringDescription(const Order& order); // Used in FileReader::SaveMapFile()
        void Execute(Order& order);
        bool Update(const Order& order); // Rename
        void Stop(const Order& order); // Rename ?
};
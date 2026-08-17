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

struct FrameTextOrder { // Display text in a frame
    std::vector<std::string> texts;
};

// UiDialogBox inherits from UiFrameText
struct DialogTextOrder { // Display text in a frame with a faceset
    std::vector<std::string> texts;
    std::string facePath;
};

struct NpcGoToOrder { // Gives MapGoToBehaviour to an NPC, which makes it move toward its target position
    // Should be the same Order for Player (for now only NPC)
    MapPosition targetPosition;
    unsigned int idNpc;
    bool isDone;
};

struct PlayCinematicOrder { // Execute Orders defined in a file given by cinematicFilepath
    std::string cinematicFilepath;
};

// TODO :
// struct AddInventoryOrder {};
// Orders about camera

using Order = std::variant<
    FrameTextOrder,
    DialogTextOrder,
    NpcGoToOrder,
    PlayCinematicOrder
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
        static std::string GetStringFromOrder(const PlayCinematicOrder& o);

        void ExecuteOrder(const FrameTextOrder& o);
        void ExecuteOrder(const DialogTextOrder& o);
        void ExecuteOrder(const NpcGoToOrder& o);
        void ExecuteOrder(const PlayCinematicOrder& o);

        // Rename
        bool UpdateOrder(const FrameTextOrder& o);
        bool UpdateOrder(const DialogTextOrder& o);
        bool UpdateOrder(const NpcGoToOrder& o);
        bool UpdateOrder(const PlayCinematicOrder& o);

        void StopOrder(const FrameTextOrder& o);
        void StopOrder(const DialogTextOrder& o);
        void StopOrder(const NpcGoToOrder& o);
        void StopOrder(const PlayCinematicOrder& o);

    public:
        OrderController(UiComponentController& uiComponentController);

        const Order& GetCurrentOrder() const;
        Order& GetCurrentOrder();
        
        static std::string GetStringDescription(const Order& order); // Used in FileReader::SaveMapFile()
        void Execute(Order& order);
        bool Update(const Order& order); // Rename
        void Stop(const Order& order); // Rename ?
};
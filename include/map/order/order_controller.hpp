#pragma once

#include <queue>
#include <string>
#include <vector>

#include "core/notifier.hpp"
#include "map/order/order.hpp"

struct Time;
class Boxing;
class Camera;
class FileReader;
class MapElementController;
class Tilemap;
class UiComponentController;
class UiDialogBox;
class UiFrameText;

enum class OrderExecutionEvent // TODO : Should not exist ?
{
    End
};

class OrderController : public Notifier<OrderExecutionEvent>
{
    private:
        std::queue<Order> m_orders;
        Order m_currentOrder;
        bool m_hasCurrentOrder;

        Boxing& m_boxing;
        Camera& m_camera;
        FileReader& m_fileReader;
        MapElementController& m_mapElementController;
        Tilemap& m_tilemap; // TODO : Should be in NPC instead of here ?
        Time& m_time;
        UiComponentController& m_uiComponentController;

        void ExecuteOrder(const FrameTextOrder& o);
        void ExecuteOrder(const DialogTextOrder& o);
        void ExecuteOrder(const NpcGoToOrder& o);
        void ExecuteOrder(const NpcFollowOrder& o);
        void ExecuteOrder(const NpcIdleOrder& o);
        void ExecuteOrder(const PlayCinematicOrder& o);
        void ExecuteOrder(const CameraSlideToPositionOrder& o);
        void ExecuteOrder(const CameraSlideToEntityOrder& o);
        void ExecuteOrder(const CameraAnchorEntityOrder& o);
        void ExecuteOrder(const EntityOrientationOrder& o);
        void ExecuteOrder(const EntityCreateOrder& o);
        void ExecuteOrder(const EntityDeleteOrder& o);
        void ExecuteOrder(const TimeDelayOrder& o);
        void ExecuteOrder(const LoadMapOrder& o);
        void ExecuteOrder(const BoxingAnimationOrder& o);

        // Rename IsOrderDone() (return true if the Order is done) ?
        bool UpdateOrder(const Order& o); // Default when there is no function with the specific Order type
        bool UpdateOrder(const FrameTextOrder& o);
        bool UpdateOrder(const DialogTextOrder& o);
        bool UpdateOrder(const NpcGoToOrder& o);
        bool UpdateOrder(const CameraSlideToPositionOrder& o);
        bool UpdateOrder(const CameraSlideToEntityOrder& o); // Try to merge with UpdateOrder(CameraSlideToPosition) ?
        bool UpdateOrder(const TimeDelayOrder& o);
        bool UpdateOrder(const BoxingAnimationOrder& o);

        void StopOrder(const Order& o); // Default when there is no function with the specific Order type
        void StopOrder(const FrameTextOrder& o);
        void StopOrder(const DialogTextOrder& o);
        void StopOrder(const NpcGoToOrder& o);
        void StopOrder(const CameraSlideToPositionOrder& o);
        void StopOrder(const CameraSlideToEntityOrder& o); // Try to merge with StopOrder(CameraSlideToPosition) ?
        void StopOrder(const TimeDelayOrder& o);
        void StopOrder(const BoxingAnimationOrder& o);

        void Execute(Order& order);
        bool Update(const Order& order); // Rename IsOrderDone() ?
        void Stop(const Order& order); // Rename ?

    public:
        OrderController(Boxing& boxing, Camera& camera, FileReader& fileReader, MapElementController& mapElementController,
            Tilemap& tilemap, Time& time, UiComponentController& uiComponentController);

        bool HasNoOrders() const;
        bool GetHasCurrentOrder() const; 
        // Even if the queue m_orders is empty, there may still be an Order (the last one removed from the queue) that is currently being executed
        // That's why HasNoOrders is not enough to know if the OrderController has finished
        
        void AddOrders(const std::vector<Order>& orders);
        void NextOrder();
};
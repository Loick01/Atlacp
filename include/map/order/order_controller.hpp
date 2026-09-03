#pragma once

#include <queue>
#include <string>
#include <vector>

#include "map/order/order.hpp"

struct Time;
class Camera;
class FileReader;
class MapElementController;
class Tilemap;
class UiComponentController;
class UiDialogBox;
class UiFrameText;

class OrderController
{
    private:
        std::queue<Order> m_orders;
        Order m_currentOrder;
        bool m_hasCurrentOrder;

        Camera& m_camera;
        FileReader& m_fileReader;
        MapElementController& m_mapElementController;
        Tilemap& m_tilemap; // Should be in NPC instead of here ?
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

        // Rename IsOrderDone() (return true if the Order is done) ?
        bool UpdateOrder(const Order& o); // Default when there is no function with the specific Order type
        bool UpdateOrder(const FrameTextOrder& o);
        bool UpdateOrder(const DialogTextOrder& o);
        bool UpdateOrder(const NpcGoToOrder& o);
        bool UpdateOrder(const CameraSlideToPositionOrder& o);
        bool UpdateOrder(const CameraSlideToEntityOrder& o); // Try to merge with UpdateOrder(CameraSlideToPosition) ?
        bool UpdateOrder(const TimeDelayOrder& o);

        void StopOrder(const Order& o); // Default when there is no function with the specific Order type
        void StopOrder(const FrameTextOrder& o);
        void StopOrder(const DialogTextOrder& o);
        void StopOrder(const NpcGoToOrder& o);
        void StopOrder(const CameraSlideToPositionOrder& o);
        void StopOrder(const CameraSlideToEntityOrder& o); // Try to merge with StopOrder(CameraSlideToPosition) ?
        void StopOrder(const TimeDelayOrder& o);

        void Execute(Order& order);
        bool Update(const Order& order); // Rename IsOrderDone() ?
        void Stop(const Order& order); // Rename ?

    public:
        OrderController(Camera& camera, FileReader& fileReader, MapElementController& mapElementController,
            Tilemap& tilemap, Time& time, UiComponentController& uiComponentController);

        void AddOrders(const std::vector<Order>& orders);
        bool NextOrder();
};
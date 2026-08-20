#pragma once

#include <queue>
#include <string>
#include <vector>

#include "map/order/order.hpp"

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

        FileReader& m_fileReader;
        MapElementController& m_mapElementController;
        Tilemap& m_tilemap; // Should be in NPC instead of here ?
        UiComponentController& m_uiComponentController;

        void ExecuteOrder(const FrameTextOrder& o);
        void ExecuteOrder(const DialogTextOrder& o);
        void ExecuteOrder(const NpcGoToOrder& o);
        void ExecuteOrder(const PlayCinematicOrder& o);

        // Rename IsOrderDone() (return true if the Order is done) ?
        bool UpdateOrder(const Order& o); // Default when there is no function with the specific Order type
        bool UpdateOrder(const FrameTextOrder& o);
        bool UpdateOrder(const DialogTextOrder& o);
        bool UpdateOrder(const NpcGoToOrder& o);

        void StopOrder(const Order& o); // Default when there is no function with the specific Order type
        void StopOrder(const FrameTextOrder& o);
        void StopOrder(const DialogTextOrder& o);
        void StopOrder(const NpcGoToOrder& o);

        void Execute(Order& order);
        bool Update(const Order& order); // Rename IsOrderDone() ?
        void Stop(const Order& order); // Rename ?

    public:
        OrderController(FileReader& fileReader, MapElementController& mapElementController, Tilemap& tilemap, UiComponentController& uiComponentController);

        void AddOrders(const std::vector<Order>& orders);
        bool NextOrder();
};
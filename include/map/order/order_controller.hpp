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

        // Rename IsOrderDone() ? Return true if the Order is done
        bool UpdateOrder(const FrameTextOrder& o);
        bool UpdateOrder(const DialogTextOrder& o);
        bool UpdateOrder(const NpcGoToOrder& o);
        bool UpdateOrder(const PlayCinematicOrder& o);

        void StopOrder(const FrameTextOrder& o);
        void StopOrder(const DialogTextOrder& o);
        void StopOrder(const NpcGoToOrder& o);
        void StopOrder(const PlayCinematicOrder& o);

        void Execute(Order& order);
        bool Update(const Order& order); // Rename IsOrderDone() ?
        void Stop(const Order& order); // Rename ?

    public:
        OrderController(FileReader& fileReader, MapElementController& mapElementController, Tilemap& tilemap, UiComponentController& uiComponentController);

        void AddOrders(const std::vector<Order>& orders);
        bool NextOrder();
};
#pragma once

#include <vector>

#include "map/interaction/order.hpp" // Order
#include "map/map_types.hpp" // MapPosition, Direction

class Tilemap;

class MapElement
{
    private:
        std::vector<Order> m_orders;
        MapPosition m_mapPosition;
        MapPosition m_targetPosition; // Should use orientation instead ? 

    protected:
        Tilemap& m_tilemap;

    public:
        MapElement(Tilemap& tilemap); // Will be protected ?
        virtual ~MapElement() = default;

        std::vector<Order>& GetOrders(); // Should be const
        MapPosition GetMapPosition() const;
        MapPosition GetTargetPosition() const;
        void SetMapPosition(const MapPosition mp);
        void SetTargetPosition(const MapPosition target);
        void SetOrders(const std::vector<Order>& orders);

        virtual void OnInteracting(const Direction direction);
        virtual void ReleaseInteracting();
};
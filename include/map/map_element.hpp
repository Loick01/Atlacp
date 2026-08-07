#pragma once

#include <vector>

#include "map/interaction/order.hpp"
#include "map/map_types.hpp" // MapPosition, Direction

class Tilemap;

class MapElement
{
    private:
        MapPosition m_mapPosition;
        MapPosition m_targetPosition; // Should use orientation instead ? 

    protected:
        Tilemap& m_tilemap;

    public:
        std::vector<Order> m_orders;
        MapElement(Tilemap& tilemap); // Will be protected ?
        virtual ~MapElement() = default;

        MapPosition GetMapPosition() const;
        MapPosition GetTargetPosition() const;
        void SetMapPosition(const MapPosition mp);
        void SetTargetPosition(const MapPosition target);
        void SetOrders(const std::vector<Order>& orders);

        virtual void OnInteracting(const Direction direction);
        virtual void ReleaseInteracting();
};
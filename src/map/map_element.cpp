#include "map/map_element.hpp"

#include "tile/tilemap.hpp"

MapElement::MapElement(Tilemap& tilemap):
    m_tilemap(tilemap)
{}

const std::vector<Order>& MapElement::GetOrders() const
{
    return m_orders;
}

MapPosition MapElement::GetMapPosition() const
{
    return m_mapPosition;
} 

MapPosition MapElement::GetTargetPosition() const
{
    return m_targetPosition;
} 

void MapElement::SetMapPosition(const MapPosition mp)
{
    m_mapPosition = mp;
}

void MapElement::SetTargetPosition(const MapPosition target)
{
    m_targetPosition = target;
}

// Should have instead SetOrders(std::vector<Order> orders) { m_orders = std::move(orders); } ? 
void MapElement::SetOrders(const std::vector<Order>& orders)
{
    m_orders = orders;
}

void MapElement::OnInteracting(const Direction direction)
{}

void MapElement::ReleaseInteracting()
{}
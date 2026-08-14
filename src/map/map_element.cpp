#include "map/map_element.hpp"

#include "tile/tilemap.hpp"

MapElement::MapElement(Tilemap& tilemap):
    m_tilemap(tilemap)
{}

std::vector<Order>& MapElement::GetOrders()
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

// https://stackoverflow.com/questions/44326522/setter-for-stdvector?rq=3
void MapElement::SetOrders(std::vector<Order> orders)
{
    m_orders = std::move(orders);
}

void MapElement::OnInteracting(const Direction direction)
{}

void MapElement::ReleaseInteracting()
{}
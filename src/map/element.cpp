#include "map/map_element.hpp"

#include "tile/tilemap.hpp"

MapElement::MapElement(Tilemap& tilemap):
    m_tilemap(tilemap)
{}

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
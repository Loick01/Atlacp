#include "element.hpp"

MapElement::MapElement(Tilemap& tilemap):
    m_tilemap(tilemap)
{
    
}

MapPosition MapElement::GetMapPosition() const
{
    return m_map_position;
} 

void MapElement::SetMapPosition(const MapPosition mp)
{
    m_map_position = mp;
}
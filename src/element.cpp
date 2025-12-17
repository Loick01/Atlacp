#include "element.hpp"

MapElement::MapElement(Tilemap& tilemap):
    m_tilemap(tilemap)
{
    
}

MapPosition MapElement::GetMapPosition() const
{
    return m_map_position;
} 

ScenePosition MapElement::GetDisplayOffset() const
{
    return m_display_offset;
}   

void MapElement::SetDisplayOffset(const ScenePosition offset)
{
    m_display_offset = offset;
}

void MapElement::SetMapPosition(const MapPosition mp)
{
    m_map_position = mp;
}
#include "element.hpp"

MapElement::MapElement(const MapPosition p, const float speed):
    m_map_position(p), m_is_free(true), m_speed(speed)
{

}

MapElement::~MapElement()
{

}
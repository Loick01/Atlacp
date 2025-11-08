#pragma once

#include <iostream>

#include "type.hpp"

class MapElement // Will be use for Player, NPC, Monster, ...
{
    protected:
        MapElement(const MapPosition p);
        ~MapElement();

        MapPosition m_map_position;

    public:
        virtual void Update() = 0;
};
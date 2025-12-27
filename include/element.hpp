#pragma once

#include "animation.hpp"
#include "tilemap.hpp"
#include "type.hpp"

class MapElement
{
    private:
        MapPosition m_map_position;

    protected:
        Tilemap& m_tilemap;
        
        MapElement(Tilemap& tilemap);
        void SetMapPosition(const MapPosition mp);

    public:
        MapPosition GetMapPosition() const; // Should be protected (will not be called in main.cpp)
};
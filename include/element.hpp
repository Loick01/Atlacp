#pragma once

#include "animation.hpp"
#include "tilemap.hpp"
#include "type.hpp"

class MapElement
{
    private:
        MapPosition m_mapPosition;
        MapPosition m_targetPosition;

    protected:
        Tilemap& m_tilemap;
        
        MapElement(Tilemap& tilemap);
        void SetMapPosition(const MapPosition mp);

    public:
        MapPosition GetMapPosition() const;
        MapPosition GetTargetPosition() const;
        void SetTargetPosition(const MapPosition target);
};
#pragma once

#include "system/type.hpp"

class Tilemap;

class MapElement
{
    private:
        MapPosition m_mapPosition;
        MapPosition m_targetPosition; // Should use orientation instead ? 

    protected:
        Tilemap& m_tilemap;
        
        MapElement(Tilemap& tilemap);
        void SetMapPosition(const MapPosition mp);

    public:
        MapPosition GetMapPosition() const;
        MapPosition GetTargetPosition() const;
        void SetTargetPosition(const MapPosition target);
};
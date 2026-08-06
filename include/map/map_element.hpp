#pragma once

#include "map/map_types.hpp" // MapPosition, Direction

class Tilemap;

class MapElement
{
    private:
        MapPosition m_mapPosition;
        MapPosition m_targetPosition; // Should use orientation instead ? 

    protected:
        Tilemap& m_tilemap;

    public:
        MapElement(Tilemap& tilemap); // Will be protected ?
        virtual ~MapElement() = default;

        MapPosition GetMapPosition() const;
        MapPosition GetTargetPosition() const;
        void SetMapPosition(const MapPosition mp);
        void SetTargetPosition(const MapPosition target);

        virtual void OnInteracting(const Direction direction);
        virtual void ReleaseInteracting();
};
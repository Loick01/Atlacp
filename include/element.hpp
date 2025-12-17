#pragma once

#include "animation.hpp"
#include "tilemap.hpp"
#include "type.hpp"

class MapElement
{
    private:
        ScenePosition m_display_offset;
        MapPosition m_map_position;

    protected:
        MapElement(Tilemap& tilemap);

        ScenePosition GetDisplayOffset() const;
        void SetMapPosition(const MapPosition mp);
        void SetDisplayOffset(const ScenePosition offset);

        Tilemap& m_tilemap;

    public:
        MapPosition GetMapPosition() const; // Should be protected (will not be called in main.cpp)
};
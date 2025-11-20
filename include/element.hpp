#pragma once

#include <iostream>

#include "tilemap.hpp"
#include "type.hpp"

class MapMovement
{
    private:
        MapPosition m_move;
        MapDirection m_direction;

    public:
        MapMovement();
        ~MapMovement();

        MapPosition GetMove() const;
        MapDirection GetDirection() const;
        void DefineMovement(MapDirection direction);
};

class MapElement // Will be use for Player, NPC, Monster, ...
{
    protected:
        MapElement(Tilemap* tilemap, const MapPosition p, const float speed);
        ~MapElement();

        Tilemap* m_tilemap; // Try to make it const ?

        MapPosition m_map_position;
        float m_speed;
        bool m_is_free;

        // Will be removed (4 lines)
        ScenePosition m_start_position;
        ScenePosition m_end_position;
        float m_progress;
        uint32_t m_last_time;

    public:
        virtual void Update() = 0;
        void StartMovement(const MapPosition movement);
};
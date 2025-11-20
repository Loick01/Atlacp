#pragma once

#include <iostream>

#include "type.hpp"

class MapElement // Will be use for Player, NPC, Monster, ...
{
    protected:
        MapElement(const MapPosition p, const float speed);
        ~MapElement();

        MapPosition m_map_position;
        bool m_is_free;

        // Will be removed (5 lines)
        ScenePosition m_start_pos;
        ScenePosition m_end_pos;
        float m_speed;
        float m_progress;
        uint32_t m_last_time;

    public:
        virtual void Update() = 0;
};
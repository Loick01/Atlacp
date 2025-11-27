#pragma once

#include "animation.hpp"
#include "tilemap.hpp"
#include "type.hpp"

enum class ElementState
{
    Free,
    Moving,
    StopMoving
};

class MapMovement
{
    private:
        MapPosition m_move;
        MapDirection m_direction;

        ScenePosition m_start_position;
        ScenePosition m_end_position;
        float m_progress;
        uint32_t m_last_time;

    public:
        MapMovement();
        ~MapMovement();

        MapPosition GetMove() const;
        MapDirection GetDirection() const;
        ScenePosition GetScenePosition() const;
        ElementState UpdateProgress(const float speed); // Return the new state the element should have
        void ResetProgress(); // Set m_progress to 0
        void DefineMovement(MapDirection direction);
        void Initialize(const int tile_size, const MapPosition start_position, const MapPosition end_position);
};

class MapElement // Will be use for Player, NPC, Monster.
{
    private:
        Tilemap* m_tilemap;
        float m_speed;

    protected:
        MapElement(Tilemap* tilemap, const float speed, const int animation_step, const float frame_duration, const Pair<int> spritesheet_size);
        ~MapElement();

        Animation m_animation; // For now, every MapElement have an animation but it will not be the case in the future 
        ElementState m_state; // Should be in Animation class ?
        MapPosition m_map_position;
        MapMovement m_current_movement;

    public:
        ScenePosition ContinueMovement(MapMovement& movement);
        virtual void Update() = 0;
        void StartMovement(MapMovement& movement, const bool is_first_movement);
};
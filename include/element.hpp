#pragma once

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
        float GetProgress() const;
        ElementState UpdateProgress(const float speed); // Return the new state the element should have
        void ResetProgress(); // Set m_progress to 0
        void DefineMovement(MapDirection direction);
        void Initialize(const int tile_size, const MapPosition start_position, const MapPosition end_position);
};

class MapElement // Will be use for Player, NPC, Monster, ...
{
    private:
        Tilemap* m_tilemap;
        float m_speed;

    protected:
        MapElement(Tilemap* tilemap, const float speed);
        ~MapElement();

        MapPosition m_map_position;
        MapMovement m_current_movement;
        ElementState m_state;

    public:
        ScenePosition ContinueMovement(MapMovement& movement);
        virtual void Update() = 0;
        void StartMovement(MapMovement& movement);
};
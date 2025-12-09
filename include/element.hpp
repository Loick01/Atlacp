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

    public:
        MapMovement();
        ~MapMovement();

        MapPosition GetMove() const;
        MapDirection GetDirection() const;
        ScenePosition GetScenePosition() const;
        ElementState UpdateProgress(const float speed, const float delta_time); // Return the new state the element should have
        void ResetProgress();
        void DefineMovement(MapDirection direction);
        void Initialize(const int tile_size, const MapPosition start_position, const MapPosition end_position);
};

class MapElement // Will be use for Player, NPC, Monster.
{
    private:
        MapMovement m_current_movement;
        float m_speed;

    protected:
        MapElement(const FileReader* file_reader, const std::string& sprite_filepath, Tilemap* tilemap, const float speed);
        ~MapElement();

        Animation m_animation; // For now, every MapElement have an animation but it will not be the case in the future 
        ElementState m_state; // Should be in Animation class ?
        MapPosition m_map_position;
        Tilemap* m_tilemap; // Should be private ?

    public:
        ScenePosition ContinueMovement(const float delta_time);
        virtual void Update(const float delta_time) = 0;
        void StartMovement(const MapMovement movement, const bool is_first_movement);
};
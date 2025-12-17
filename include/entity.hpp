#pragma once

#include <iostream>

#include "drawable.hpp"
#include "element.hpp"

enum class EntityState
{
    Free,
    Moving,
    OnStop
};

class EntityMovement
{
    private:
        MapPosition m_move;
        MapDirection m_direction;

        MapPosition m_start_map_position;
        ScenePosition m_start_position;
        ScenePosition m_end_position;
        float m_progress;

    public:
        EntityMovement();
        ~EntityMovement();

        MapPosition GetMove() const;
        MapDirection GetDirection() const;
        MapPosition GetStartPosition() const;
        ScenePosition GetScenePosition() const;
        EntityState UpdateProgress(const float speed, const float delta_time); // Return the new state the entity should have
        
        // Not sure to keep these 2 functions
        MapPosition GetMoveFromDirection(const MapDirection direction) const;
        MapDirection GetDirectionFromMove(const MapPosition move) const;

        void DefineMovement(const MapDirection direction);
        void Initialize(const int tile_size, const MapPosition start_position, const MapPosition end_position);
};

class Entity : public Drawable, public MapElement
{
    private:
        EntityMovement m_current_movement;
        Animation m_animation;
        EntityState m_state; // Should be in Animation class ?
        float m_speed;

        ScenePosition ContinueMovement(const float delta_time);
        void TryStartMovement(const EntityMovement movement, const bool is_first_movement, const bool can_exit_map);

    protected:
        Entity(TextureController& texture_controller, const std::string& sprite_filepath, Camera& camera, const FileReader& file_reader,
            Tilemap& tilemap, const float speed);
        
        ScenePosition GetFinalDrawingPosition(const ScenePosition sp) const;
        void SetState(const EntityState state);
    
    public:
        ~Entity();
        virtual void Update(const float delta_time) = 0;
        void DrawTexture() const override;

        void OrderStartMovement(const MapDirection direction, const bool is_first_movement, const bool can_exit_map=false); 
        void OrderUpdateMovement(const float delta_time);

        EntityState GetState() const;
        EntityMovement GetCurrentMovement() const;
        float GetSpeed() const;
        void Reset();
};
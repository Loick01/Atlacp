#pragma once

#include <iostream>

#include "drawable.hpp"
#include "element.hpp"

enum class EntityState
{
    Free,
    Moving,
    StopMoving
};

class EntityMovement
{
    private:
        MapPosition m_move;
        MapDirection m_direction;

        ScenePosition m_start_position;
        ScenePosition m_end_position;
        float m_progress;

    public:
        EntityMovement();
        ~EntityMovement();

        MapPosition GetMove() const;
        MapDirection GetDirection() const;
        ScenePosition GetScenePosition() const;
        EntityState UpdateProgress(const float speed, const float delta_time); // Return the new state the element should have
        void ResetProgress();
        void DefineMovement(MapDirection direction);
        void Initialize(const int tile_size, const MapPosition start_position, const MapPosition end_position);
};

class Entity : public Drawable, public MapElement
{
    private:
        EntityMovement m_current_movement;
        Animation m_animation;
        EntityState m_state; // Should be in Animation class ?
        float m_speed;

    protected:
        Entity(TextureController* texture_controller, const std::string& sprite_filepath, Camera* camera, const FileReader* file_reader,
            Tilemap* tilemap, const float speed);
        ~Entity();
        
        EntityState GetState() const;
        void SetState(const EntityState state);
        ScenePosition GetFinalDrawingPosition(const ScenePosition sp) const;

        ScenePosition ContinueMovement(const float delta_time);
        void StartMovement(const EntityMovement movement, const bool is_first_movement, const bool can_exit_map=false);
        void Reset();
    
    public:
        virtual void Update(const float delta_time) = 0;
        void DrawTexture() const override;
};
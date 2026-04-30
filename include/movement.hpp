#pragma once

#include "type.hpp"

enum class EntityState // Should be called MovementState (and I should add AnimationState ?)
{
    Free, Moving, OnStop, Interacting
};

class EntityMovement
{
    private:
        MapPosition m_move;
        Direction m_direction;

        MapPosition m_startMapPosition;
        ScenePosition m_startPosition;
        ScenePosition m_endPosition;
        float m_progress;

    public:
        EntityMovement();

        MapPosition GetMove() const;
        Direction GetDirection() const;
        Direction GetOppositeDirection() const;
        MapPosition GetStartPosition() const;
        ScenePosition GetScenePosition() const;
        EntityState UpdateProgress(const float speed, const float deltaTime); // Return the new state the entity should have
        
        // Not sure to keep these 2 functions
        MapPosition GetMoveFromDirection(const Direction direction) const;
        Direction GetDirectionFromMove(const MapPosition move) const;

        void DefineMovement(const Direction direction);
        void Initialize(const int tileSize, const MapPosition startPosition, const MapPosition endPosition);
};
#pragma once

#include "entity.hpp"
#include "pathfind.hpp"
#include "type.hpp"

class EntityBehaviour // Should be call EntityMovementBehaviour or MovementBehaviour ?
{
    protected:
        Random m_random; // Should not be here ? (a behaviour will not necessary need random generation)

    public:
        EntityBehaviour();

        // Must have one function for each value in Entity::EntityState
        virtual void FreeCase(Entity& entity, const float delta_time) = 0;
        virtual void MovingCase(Entity& entity, const float delta_time) = 0;
        virtual void OnStopCase(Entity& entity) = 0;
};

class RandomBehaviour : public EntityBehaviour // Always try to move in random direction 
{
    private:
        float m_delay; // Remaining time before a new movement

    public:
        RandomBehaviour(); // Remove ?
        void FreeCase(Entity& entity, const float delta_time) override;
        void MovingCase(Entity& entity, const float delta_time) override;
        void OnStopCase(Entity& entity) override;
};

class FollowEntityBehaviour : public EntityBehaviour
{
    // Assume the tracked entity is one tile away from the follower. Later I will make a follow behaviour with pathfiding
    private:
        const Entity* m_tracked_entity;

    public:
        FollowEntityBehaviour(const Entity* tracked_entity, const float follower_speed);

        void FreeCase(Entity& entity, const float delta_time) override;
        void MovingCase(Entity& entity, const float delta_time) override;
        void OnStopCase(Entity& entity) override;
};

class GoToBehaviour : public EntityBehaviour
{
    // Entity with this behaviour will go to a given position, following the MapPosition inside m_path
    private:
        std::vector<MapPosition> m_path;
        unsigned int m_path_index;

    public:
        GoToBehaviour(const MapPosition start_position, const MapPosition end_position, const Tilemap& tilemap);

        void FreeCase(Entity& entity, const float delta_time) override;
        void MovingCase(Entity& entity, const float delta_time) override;
        void OnStopCase(Entity& entity) override;
};
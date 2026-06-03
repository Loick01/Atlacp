#pragma once

#include "system/type.hpp"

class MapEntity;
class Tilemap;

class MapEntityBehaviour // Should be call EntityMovementBehaviour or MovementBehaviour ?
{
    protected:
        Random m_random; // Should not be here ? (a behaviour will not necessary need random generation)

    public:
        MapEntityBehaviour() = default;
        virtual ~MapEntityBehaviour() = default;
        
        // One function for each value in MapEntity::EntityState
        virtual void FreeCase(MapEntity& entity, const float deltaTime) = 0;
        virtual void MovingCase(MapEntity& entity, const float deltaTime) = 0;
        virtual void OnStopCase(MapEntity& entity) = 0;
};

class RandomBehaviour : public MapEntityBehaviour // Always try to move in random direction 
{
    private:
        float m_delay; // Remaining time before a new movement

    public:
        RandomBehaviour();
        void FreeCase(MapEntity& entity, const float deltaTime) override;
        void MovingCase(MapEntity& entity, const float deltaTime) override;
        void OnStopCase(MapEntity& entity) override;
};

class FollowEntityBehaviour : public MapEntityBehaviour
{
    // Assume the tracked entity is one tile away from the follower. Later I will make a follow behaviour with pathfiding
    private:
        const MapEntity* m_trackedEntity; // Could be MapEntity& when I remove trackedEntity from NPC constructor ?

    public:
        FollowEntityBehaviour(const MapEntity* trackedEntity, const float followerWalkSpeed);

        void FreeCase(MapEntity& entity, const float deltaTime) override;
        void MovingCase(MapEntity& entity, const float deltaTime) override;
        void OnStopCase(MapEntity& entity) override;
};

class GoToBehaviour : public MapEntityBehaviour
{
    // MapEntity with this behaviour will go to a given position, following the MapPosition inside m_path
    private:
        std::vector<MapPosition> m_path;
        unsigned int m_pathIndex;

    public:
        GoToBehaviour(const MapPosition startPosition, const MapPosition endPosition, const Tilemap& tilemap);

        void FreeCase(MapEntity& entity, const float deltaTime) override;
        void MovingCase(MapEntity& entity, const float deltaTime) override;
        void OnStopCase(MapEntity& entity) override;
};
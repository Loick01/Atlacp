#pragma once

#include <vector>

#include "core/notifier.hpp"
#include "core/random.hpp"
#include "map/map_types.hpp" // MapPosition

class MapEntity;
class Tilemap;

class MapEntityBehaviour // TODO : Should be call EntityMovementBehaviour or MovementBehaviour ?
{
    protected:
        Random m_random; // TODO : Should not be here ? (a behaviour will not necessary need random generation)
        bool m_init;

    public:
        MapEntityBehaviour();
        virtual ~MapEntityBehaviour() = default;
        
        bool IsInit() const;
        
        // One function for each value in MapEntity::EntityMovementState
        virtual void FreeCase(MapEntity& entity, const float deltaTime) = 0;
        virtual void MovingCase(MapEntity& entity, const float deltaTime) = 0;
        virtual void OnStopCase(MapEntity& entity) = 0;
};

class MapRandomBehaviour : public MapEntityBehaviour // Always try to move in random direction 
{
    private:
        float m_delay; // Remaining time before a new movement

    public:
        MapRandomBehaviour();
        void FreeCase(MapEntity& entity, const float deltaTime) override;
        void MovingCase(MapEntity& entity, const float deltaTime) override;
        void OnStopCase(MapEntity& entity) override;
};

class MapFollowBehaviour : public MapEntityBehaviour // TODO : Rename MapFollowEntitybehaviour ?
{
    // Assume the tracked entity is one tile away from the follower. TODO : Follow behaviour with pathfiding
    private:
        const MapEntity* m_trackedEntity; // TODO++ : Could be MapEntity& when I remove trackedEntity from NPC constructor ?

    public:
        MapFollowBehaviour(const MapEntity* followerEntity, const MapEntity* trackedEntity);
         
        void FreeCase(MapEntity& entity, const float deltaTime) override;
        void MovingCase(MapEntity& entity, const float deltaTime) override;
        void OnStopCase(MapEntity& entity) override;
};

class MapGoToBehaviour : public MapEntityBehaviour, public Notifier<UselessEvent> // Will Notify when reaching target for Order execution
{
    // MapEntity with this behaviour will go to a given position, following the MapPosition inside m_path
    private:
        std::vector<MapPosition> m_path;
        unsigned int m_pathIndex;

        // TODO : Add bool isRunning

    public:
        MapGoToBehaviour(const MapPosition startPosition, const MapPosition endPosition, const Tilemap& tilemap);

        bool IsDone() const;
        
        void FreeCase(MapEntity& entity, const float deltaTime) override;
        void MovingCase(MapEntity& entity, const float deltaTime) override;
        void OnStopCase(MapEntity& entity) override;
};

class MapIdleBehaviour : public MapEntityBehaviour
{
    private:
        Direction m_direction;

    public:
        MapIdleBehaviour(MapEntity& entity, const Direction direction);

        void FreeCase(MapEntity& entity, const float deltaTime) override;
        void MovingCase(MapEntity& entity, const float deltaTime) override;
        void OnStopCase(MapEntity& entity) override;
};
#pragma once

#include <vector>

#include "map/map_types.hpp"

class Tilemap;

class Pathfind // Singleton
{
    private:
        Pathfind() = default;

    public:
        static Pathfind& GetInstance();
        std::vector<MapPosition> ComputePath(const MapPosition startPosition, const MapPosition endPosition, const Tilemap& tilemap) const;
};
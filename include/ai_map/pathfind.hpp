#pragma once

#include <vector>
#include <queue>

#include "system/type.hpp"

class Tilemap;

class Pathfind // Singleton
{
    private:
        Pathfind() = default;

    public:
        static Pathfind& GetInstance();
        std::vector<MapPosition> ComputePath(const MapPosition startPosition, const MapPosition endPosition, const Tilemap& tilemap) const;
};
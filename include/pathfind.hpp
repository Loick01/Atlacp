#pragma once

#include <vector>
#include <queue>

#include "tilemap.hpp"
#include "type.hpp"

class Pathfind // Singleton
{
    private:
        Pathfind() = default;

    public:
        static Pathfind& GetInstance();
        std::vector<MapPosition> ComputePath(const MapPosition startPosition, const MapPosition endPosition, const Tilemap& tilemap) const;
};
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
        std::vector<MapPosition> ComputePath(const MapPosition start_position, const MapPosition end_position, const Tilemap& tilemap) const;
};
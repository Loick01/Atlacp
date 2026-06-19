#pragma once

#include <vector>

#include "map/map_types.hpp"

class Tilemap;

class Pathfind
{
    public:
        // Don't pass MapPosition parameters as const&
        static std::vector<MapPosition> ComputePath(const MapPosition startPosition, const MapPosition endPosition, const Tilemap& tilemap);
};
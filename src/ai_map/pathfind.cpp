#include "ai_map/pathfind.hpp"

Pathfind& Pathfind::GetInstance() {
    static Pathfind instance; // Local static object, create an instance only when this is the first call
    return instance;
}

std::vector<MapPosition> Pathfind::ComputePath(const MapPosition startPosition, const MapPosition endPosition, const Tilemap& tilemap) const
{
    // Breadth First Search -> Explore all the accessible tiles
    std::queue<MapPosition> queue;
    queue.push(startPosition);
    std::vector<bool> visited(tilemap.GetGridSize(), false);
    visited[tilemap.GetTileIndex(startPosition)] = true; 
    std::vector<MapPosition> parents(tilemap.GetGridSize());
    std::vector<MapPosition> directions = {{1,0}, {-1,0}, {0,1}, {0,-1}}; // Should get it from EventController ?
    bool found = false;

    while (!queue.empty()){
        const MapPosition current = queue.front();
        queue.pop();
        if (current == endPosition){
            found = true;
            break;
        }
        for (const MapPosition direction : directions){
            const MapPosition neighbour = current + direction;
            if (tilemap.IsOutOfMap(neighbour) != MapBound::Inside)
                continue;
            const int neighborIndex = tilemap.GetTileIndex(neighbour);
            if (!tilemap.GetOccupancyGrid()[neighborIndex] || visited[neighborIndex])
                continue;
            visited[neighborIndex] = true;
            parents[neighborIndex] = current;
            queue.push(neighbour);
        }
    }
    std::vector<MapPosition> path;
    if (!found) return path; // Will throw an error when no path can be found from startPosition to endPosition ?
    MapPosition currentPosition = endPosition;
    while (currentPosition != startPosition){
        path.push_back(currentPosition);
        currentPosition = parents[tilemap.GetTileIndex(currentPosition)];
    }
    // Do not add startPosition to path !!!
    std::reverse(path.begin(), path.end());
    return path;
}
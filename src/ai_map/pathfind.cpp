#include "ai_map/pathfind.hpp"

#include <queue>
#include <sstream>

#include "tile/tilemap.hpp"

std::vector<MapPosition> Pathfind::ComputePath(const MapPosition startPosition, const MapPosition endPosition, const Tilemap& tilemap)
{
    // Breadth First Search -> Explore all the accessible tiles
    std::queue<MapPosition> queue;
    queue.push(startPosition);
    std::vector<bool> visited(tilemap.GetGridSize(), false);
    visited[tilemap.GetTileIndex(startPosition)] = true; 
    std::vector<MapPosition> parents(tilemap.GetGridSize());
    std::vector<MapPosition> directions = {{1,0}, {-1,0}, {0,1}, {0,-1}}; // TODO++ : Should get it from EventController ?
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

    if (!found) {
        std::ostringstream message;
        message << "No path can be found from " << startPosition << " to " << endPosition;
        Logger::GetInstance().AddLog(Level::L4, message.str());
        return {};
    }

    std::vector<MapPosition> path;
    MapPosition currentPosition = endPosition;
    while (currentPosition != startPosition){
        path.push_back(currentPosition);
        currentPosition = parents[tilemap.GetTileIndex(currentPosition)];
    }
    // Do not add startPosition to path !!!
    std::reverse(path.begin(), path.end());
    return path;
}
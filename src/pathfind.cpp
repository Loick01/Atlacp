#include "pathfind.hpp"

Pathfind& Pathfind::GetInstance() {
    static Pathfind instance; // Local static object, create an instance only when this is the first call
    return instance;
}

std::vector<MapPosition> Pathfind::ComputePath(const MapPosition start_position, const MapPosition end_position, const Tilemap& tilemap) const
{
    // Breadth First Search -> Explore all the accessible tiles
    std::queue<MapPosition> queue;
    queue.push(start_position);
    std::vector<bool> visited(tilemap.GetGridSize(), false);
    visited[tilemap.GetTileIndex(start_position)] = true; 
    std::vector<MapPosition> parents(tilemap.GetGridSize());
    std::vector<MapPosition> directions = {{1,0}, {-1,0}, {0,1}, {0,-1}}; // Should get it from EventController ?
    bool found = false;

    while (!queue.empty()){
        const MapPosition current = queue.front();
        queue.pop();
        if (current == end_position){
            found = true;
            break;
        }
        for (const MapPosition direction : directions){
            const MapPosition neighbour = current + direction;
            if (tilemap.IsOutOfMap(neighbour) != MapBound::Inside)
                continue;
            const int neighbour_index = tilemap.GetTileIndex(neighbour);
            if (!tilemap.GetOccupancyGrid()[neighbour_index] || visited[neighbour_index])
                continue;
            visited[neighbour_index] = true;
            parents[neighbour_index] = current;
            queue.push(neighbour);
        }
    }
    std::vector<MapPosition> path;
    if (!found) return path; // Will throw an error, no path can be found from start_position to end_position
    MapPosition current_position = end_position;
    while (current_position != start_position){
        path.push_back(current_position);
        current_position = parents[tilemap.GetTileIndex(current_position)];
    }
    // Do not add start_position to path !!!
    std::reverse(path.begin(), path.end());
    return path;
}
#pragma once

#include "map/map_types.hpp" // MapPosition
#include "map/map_element_types.hpp" // DataMapElement
#include "tile/layer.hpp" // TileLayer

struct DataMap
{
    size_t layerCount;
    size_t layerSplitIndex; // TODO : Later, each MapEntity will have its own value so that it can be drawn independently 
    std::vector<TileLayer> layers;
    std::vector<bool> occupancyGrid; // TODO : Could be a TileLayer ?
    std::vector<TextureKey> tilesets; // Tileset keys (read from the header of the map file) that will be used to build the map
    std::vector<DataMapElement> elements;
    std::vector<DataMapElement> triggers;
    MapPosition spawnPosition; // Position where the player will start when loading the associated map (-1 if no specific position)
    GridSize size; // TODO : Should not be here, and only in TileLayer ?
    std::string tilesetDirectoryPath;

    DataMap() {
        layerCount = 0;
        spawnPosition = {0, 0};
        layerSplitIndex = 0;
        occupancyGrid.clear();
        layers.clear();
    }
};
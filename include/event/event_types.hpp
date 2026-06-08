#pragma once

#include "map/map_types.hpp" // Direction
#include "tile/tile_types.hpp" // Tile

struct GameplayEventState
{
    Direction mapDirection;
    bool isInteracting;
    bool isRunning;

    GameplayEventState() {
        mapDirection = Direction::None;
        isInteracting = false;
        isRunning = false;
    }
};

struct EditorEventState
{
    Tile selectedTile;
    int selectedTileset;
    std::vector<bool> isLayerRendered;
    int selectedLayer; // Should not be higher than m_layerCount
    bool isCameraMoving;
    bool isReplacingTile;

    EditorEventState() {
        selectedLayer = 0;
        isCameraMoving = false;
        selectedTile = 0;
        selectedTileset = 0;
        isReplacingTile = false;
    }

    void SetLayerSize(const unsigned int layerCount) {
        isLayerRendered.assign(layerCount, true);
    }
};

struct BattleEventState
{
    Direction uiDirection; // Rename
    bool isAction;

    BattleEventState() {
        uiDirection = Direction::None;
        isAction = false;
    }
};

// I can't use a template on EventController, that's why I use for now this extra-class (besides I will be able to use it for other classes)
template <typename EventState>
class EventStateHolder
{
    protected:
        EventState m_eventState;

    public:
        EventStateHolder() = default;

        EventState GetEventState() const {
            return m_eventState;
        }

        void SetEventState(const EventState& eventState) {
            m_eventState = eventState;
        }
};
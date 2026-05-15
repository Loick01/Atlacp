#pragma once

#include <cstdint>
#include <iostream> // Because operator<< in this file
#include <set>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <vector>    

using TextureKey = std::string;
using Tile = unsigned int; // Use a struct ?

// Operator signature list : https://gist.github.com/beached/38a4ae52fcadfab68cb6de05403fa393

template <typename T>
struct Pair
{
    T x;
    T y;
};

struct Vec2 : public Pair<int>
{
    bool operator==(const Vec2& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }

    bool operator!=(const Vec2& rhs ) const
    {
        return x != rhs.x || y != rhs.y;
    }

    Vec2 operator+(const Vec2 rhs) const
    {
        return {x+rhs.x, y+rhs.y};
    }

    Vec2 operator-(const Vec2 rhs) const
    {
        return {x-rhs.x, y-rhs.y};
    }

    Vec2 operator+(const int rhs) const
    {
        return {x+rhs, y+rhs};
    }

    Vec2& operator+=(const Vec2& rhs) 
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vec2 operator*(const Pair<int> rhs) const
    {
        return {x*rhs.x, y*rhs.y};
    }

    Vec2 operator*(const float rhs) const
    {
        return {static_cast<int>(x*rhs), static_cast<int>(y*rhs)};
    }

    Vec2 operator/(const float rhs) const
    {
        return {static_cast<int>(x/rhs), static_cast<int>(y/rhs)};
    }

    Pair<bool> operator>(const Vec2 rhs) const
    {
        return {x>rhs.x, y>rhs.y};
    }
};

using AreaSize = Vec2;
using GridSize = Vec2;

template <typename T>
std::ostream& operator<<(std::ostream& os, const Pair<T>& v) {
    os << "(x=" << v.x << ", y=" << v.y << ")\n";
    return os;
}

struct ScenePosition : public Vec2
{
    ScenePosition() = default;
    ScenePosition(const int px, const int py) { x = px; y = py; }
    ScenePosition(const Vec2& v) { x = v.x; y = v.y; }
};

struct ScreenPosition : public Vec2
{
    ScreenPosition() = default;
    ScreenPosition(const int px, const int py) { x = px; y = py; }
    ScreenPosition(const float px, const float py) { x = px; y = py; }
    ScreenPosition(const Vec2& v) { x = v.x; y = v.y; }
};

// Rename GridPosition ?
struct MapPosition : public Vec2
{
    MapPosition() = default;
    MapPosition(const int px, const int py) { x = px; y = py; }
    MapPosition(const Vec2& v) { x = v.x; y = v.y; }

    ScenePosition ToScenePosition(const int tileSize) const
    {
        return ScenePosition{x, y}*tileSize;
    }
};

enum class MapBound
{
    Inside,
    OutUp,
    OutDown,
    OutRight,
    OutLeft
};

enum class Direction
{
    // Do not change the order (sprite animation is based on implicit int defined in this enum)
    // Random::GetRandomDirection and EntityMovement::GetOppositeDirection also use this order
    Down, // 0
    Right, // 1
    Up, // 2
    Left, // 3
    None
};

struct WorldData
{
    std::vector<std::string> maps;
    GridSize size;
    size_t startMap;
};

struct TilesetData
{
    TextureKey tilesetKey;
    std::set<Tile> solidTiles; // Should use unordered set ?
    GridSize size;
    int tileSize;
};

struct AnimationData
{
    std::vector<Vec2> sprites; // Position of each sprites in spritesheet. Should use ScreenPosition (or something else) instead of Vec2 ?
    AreaSize spriteSize;
    int step; // How many step for the animation
    float frameDuration;
};

// DataNPC and DataUi are used by FileReader to return values used for NPC/Ui construction
struct DataNPC // Rename
{
    MapPosition position;
    std::string sprite;
    float walkSpeed;
    float runSpeed;
};

// Remove ?
struct TilesetNormalizationInfo
{
    int lastLowerBound;
    int lastUpperBound;
};

// Should not be here ?
struct Random
{
    Random()
    {
        srand(time(NULL));
    }

    float GetRandomFloat(float min, float max) const
    {
        return ((float)rand()/RAND_MAX)*(max-min)+min;
    }

    Direction GetRandomDirection() const
    {
        return static_cast<Direction>(rand()%4);
    }
};

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
#pragma once

#include "animation/sprite_animation.hpp" // SpriteAnimation
#include "map/map_types.hpp" // Direction

class MapEntityAnimation : public SpriteAnimation // Must have sprites for 4 directions
{
    private:
        int m_indexOffset;

    public:
        MapEntityAnimation(const FileReader& fileReader, const std::string& path);

        void Initialize(const Direction direction, const bool isFirstMovement); // Should be in SpriteAnimation ?
        void Reset(const Direction direction); // Rename ?
        void Continue(const float deltaTime) override;
};
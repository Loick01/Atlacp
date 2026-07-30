#pragma once

#include "animation/animation.hpp" // Animation
#include "map/map_types.hpp" // Direction

class MapEntityAnimation : public Animation // Must have sprites for 4 directions
{
    private:
        int m_indexOffset;

    public:
        MapEntityAnimation(const FileReader& fileReader, const std::string& path);

        void Initialize(const Direction direction, const bool isFirstMovement); // Should be in Animation ?
        void Reset(const Direction direction); // Rename ?
        void Continue(const float deltaTime) override;
};
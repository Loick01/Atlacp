#pragma once

#include <algorithm>

#include "type.hpp"
#include "window.hpp"

class Camera
{
    private:
        ScenePosition m_position;
        GridSize m_rangeTile; // How many tiles (width and height) will be rendered in the window. Should not be a ScenePosition
        AreaSize m_viewport; // Size of rendering area
        
        ScreenPosition m_screenOffset;
        AreaSize m_tilemapSize; // Could change when loading a new map
        Pair<bool> m_isOffScreen;
        float m_zoom;
        
        bool m_shouldCulling;
        Pair<int> m_startIndex;
        Pair<int> m_endIndex;

    public:
        Camera();

        void ComputeViewport(Window& window, const GridSize rangeTile, const int tileSize);
        ScreenPosition GetScreenOffset() const;
        ScenePosition GetPosition() const;
        AreaSize GetViewport() const;
        Pair<bool> GetIsOffScreen() const;
        Pair<int> GetStartIndex() const;
        Pair<int> GetEndIndex() const;
        float GetZoom() const;
        void AddZoom(const float z);
        void SetTilemapInfo(const AreaSize tilemapSize);
        void SetCameraPosition(const ScenePosition sp); // Set m_position to sp
        void MoveCameraPosition(const ScenePosition sp); // Add sp to m_position
        void Reset();
        void LookAt(const ScenePosition sp);

        void SetShouldCulling(const bool shouldCulling);
        void ComputeMapCulling(const GridSize layerSize, const int tileSize);
};
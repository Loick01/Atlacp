#include "camera.hpp"

Camera::Camera(Window& window, const GridSize range_tile, const int tile_size):
    m_range_tile(range_tile)
{
    m_position = ScenePosition{0,0};
    const int window_width = window.GetWidth();
    const int window_height = window.GetHeight();
    const Pair<float> best_possible_zoom = {(window_width/range_tile.x)/static_cast<float>(tile_size),
                                            (window_height/range_tile.y)/static_cast<float>(tile_size)};
    m_zoom = std::min(best_possible_zoom.x, best_possible_zoom.y);
    m_viewport = m_range_tile*tile_size*m_zoom;

    const ScenePosition outside_viewport = ScenePosition{window_width, window_height} - m_viewport;
    if (best_possible_zoom.x < best_possible_zoom.y){ // Letterboxing
        window.SetBoxing(0, window_height-outside_viewport.y/2, window_width, outside_viewport.y/2);
        m_screen_offset = ScenePosition{0, outside_viewport.y/2};
    }else{ // Pillarboxing
        window.SetBoxing(window_width-outside_viewport.x/2, 0, outside_viewport.x/2, window_height);
        m_screen_offset = ScenePosition{outside_viewport.x/2, 0};
    }
}

ScreenPosition Camera::GetScreenOffset() const
{   
    return m_screen_offset;
}

ScenePosition Camera::GetPosition() const
{
    return m_position;
}

AreaSize Camera::GetViewport() const
{
    return m_viewport;
}

GridSize Camera::GetRangeTile() const
{
    return m_range_tile;
}

Pair<int> Camera::GetStartIndex() const
{
    return m_start_index;
}

Pair<int> Camera::GetEndIndex() const
{
    return m_end_index;
}

Pair<bool> Camera::GetIsOffScreen() const
{
    return m_is_off_screen;
}

float Camera::GetZoom() const
{
    return m_zoom;
}

void Camera::AddZoom(const float z)
{
    m_zoom += z;
}   

void Camera::SetTilemapInfo(const AreaSize tilemap_size)
{
    m_tilemap_size = tilemap_size*m_zoom;
    m_is_off_screen = m_tilemap_size > m_viewport;
}

void Camera::SetCameraPosition(const ScenePosition sp)
{
    m_position = sp;
} 

void Camera::MoveCameraPosition(const ScenePosition sp)
{
    m_position += sp;
} 

void Camera::Reset() // Used in editor
{
    m_zoom = 1.0f;
    SetCameraPosition(ScenePosition{0,0});
}

// Should use 2 different version of this : x axis / y axis
void Camera::LookAt(const ScenePosition sp) // Center the camera on a scene position with checking map bounds
{
    ScenePosition camera_position = sp - m_viewport/2;
    if (m_is_off_screen.x) camera_position.x = std::clamp(camera_position.x, 0, m_tilemap_size.x - m_viewport.x); // Map must be render at ScenePosition{0,0}
    else camera_position.x = m_tilemap_size.x/2 - m_viewport.x/2;
    if (m_is_off_screen.y) camera_position.y = std::clamp(camera_position.y, 0, m_tilemap_size.y - m_viewport.y); // Map must be render at ScenePosition{0,0}
    else camera_position.y = m_tilemap_size.y/2 - m_viewport.y/2;
    SetCameraPosition(camera_position);
}

void Camera::SetShouldCulling(const bool should_culling)
{
    m_should_culling = should_culling;
}

void Camera::ComputeMapCulling(const GridSize layer_size, const int tile_size)
{
    m_start_index = GridSize{0, 0};
    m_end_index = layer_size;
    if (m_should_culling){ // No map culling in editor (find better way than just use a bool ?)
        if (m_is_off_screen.x){
            m_start_index.x = m_position.x/(tile_size*m_zoom);
            // While animating a movement, m_end_index could not be enough to fill the window with the map
            // So I add 1 to m_end_index, and check if it becomes greater than map size
            m_end_index.x = std::min(m_end_index.x, m_start_index.x + m_range_tile.x + 1);
        }
        if (m_is_off_screen.y){
            m_start_index.y = m_position.y/(tile_size*m_zoom);
            m_end_index.y = std::min(m_end_index.y, m_start_index.y + m_range_tile.y + 1);
        }
    }
}
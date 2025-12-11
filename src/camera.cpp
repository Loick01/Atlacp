#include "camera.hpp"

Camera::Camera(Window* window, const ScenePosition range_tile, const int tile_size):
    m_range_tile(range_tile)
{
    m_position = ScenePosition{0,0};
    const int window_width = window->GetWidth();
    const int window_height = window->GetHeight();
    const Pair<float> best_possible_zoom = {(window_width/range_tile.x)/static_cast<float>(tile_size),
                                            (window_height/range_tile.y)/static_cast<float>(tile_size)};
    m_zoom = std::min(best_possible_zoom.x, best_possible_zoom.y);
    m_viewport = m_range_tile*tile_size*m_zoom;

    const ScenePosition outside_viewport = ScenePosition{window_width, window_height} - m_viewport;
    if (best_possible_zoom.x < best_possible_zoom.y){ // Letterboxing
        window->SetBoxing(0, window_height-outside_viewport.y/2, window_width, outside_viewport.y/2);
        m_offset = ScenePosition{0, outside_viewport.y/2}; // Remove
    }else{ // Pillarboxing
        window->SetBoxing(window_width-outside_viewport.x/2, 0, outside_viewport.x/2, window_height);
        m_offset = ScenePosition{outside_viewport.x/2, 0}; // Remove
    }
}

Camera::~Camera()
{

}

ScenePosition Camera::GetOffset() const
{   
    return m_offset;
}

ScenePosition Camera::GetCameraPosition() const
{
    return m_position;
}

ScenePosition Camera::GetRangeTile() const
{
    return m_range_tile;
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

void Camera::SetTilemapInfo(const ScenePosition tilemap_size, const int tile_size)
{
    m_tilemap_size = tilemap_size;
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
#pragma once

#include <iostream> // operator<<

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

enum class Direction
{
    // Do not change the order (sprite animation is based on implicit int defined in this enum)
    // Random::GetRandomDirection and MapMovement::GetOppositeDirection also use this order
    Down, // 0
    Right, // 1
    Up, // 2
    Left, // 3
    None
};

inline std::string GetStringFromDirection(const Direction d) // Should be in file_utils.cpp ?
{
    switch (d) {
        case Direction::Up :
            return "up";
        case Direction::Down :
            return "down";
        case Direction::Right :
            return "right";
        case Direction::Left :
            return "left";
        default:   
            throw std::runtime_error("Unknown values as Direction");
    }
}
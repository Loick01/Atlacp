#pragma once

#include "map/order/order.hpp"
#include "map/map_types.hpp"

struct DataNPC
{
    std::vector<Order> orders;
    MapPosition position;
    std::string sprite;
    float walkSpeed;
    float runSpeed;
    unsigned int id;
};

struct DataMapElement
{
    std::vector<Order> orders;
    MapPosition position;
};
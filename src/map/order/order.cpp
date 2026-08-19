#include "map/order/order.hpp"

std::string GetStringDescription(const Order& order)
{
    return std::visit(
        [](const auto& o)
        {
            return o.GetString();
        },
        order
    );
    throw std::runtime_error("No GetString() with this type of Order"); // ?
}
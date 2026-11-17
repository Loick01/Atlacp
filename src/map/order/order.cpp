#include "map/order/order.hpp"

#include "core/logger.hpp"

std::string GetStringDescription(const Order& order)
{
    return std::visit(
        [](const auto& o)
        {
            return o.GetString();
        },
        order
    );
}
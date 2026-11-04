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
    Logger::GetInstance().AddLog(Level::L4, "No GetStringDescription() with this type of Order");
    return ""; // TODO : Should have "none" or something like that ? Then I would get a new Log in FileReader::ReadOrders each time I try to read this Order
}
#pragma once

#include "engine/types.hpp"

namespace engine {

struct Trade {
    OrderId buy_order_id;
    OrderId sell_order_id;
    Price price;
    Quantity quantity;
    Timestamp timestamp;
};

} // namespace engine
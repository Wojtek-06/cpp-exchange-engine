#pragma once

#include "engine/types.hpp"

namespace engine {

struct Order {
    OrderId id;
    Side side;
    OrderType type;
    Price price;
    Quantity quantity;
    Quantity remaining_quantity;
    Timestamp timestamp;

    Order(
        OrderId id_,
        Side side_,
        OrderType type_,
        Price price_,
        Quantity quantity_,
        Timestamp timestamp_
    )
        : id(id_),
          side(side_),
          type(type_),
          price(price_),
          quantity(quantity_),
          remaining_quantity(quantity_),
          timestamp(timestamp_)
    {}
};

} // namespace engine
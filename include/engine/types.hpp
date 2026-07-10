#pragma once

#include <cstdint>

namespace engine {

using OrderId = std::uint64_t;
using Price = std::int64_t;
using Quantity = std::uint32_t;
using Timestamp = std::uint64_t;

enum class Side {
    Buy,
    Sell
};

enum class OrderType {
    Limit,
    Market
};

} // namespace engine
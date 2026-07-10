#pragma once

#include "engine/order.hpp"
#include "engine/trade.hpp"

#include <functional>
#include <list>
#include <map>
#include <optional>
#include <unordered_map>
#include <vector>

namespace engine {

class OrderBook {
public:
    std::vector<Trade> addOrder(const Order& order);
    bool cancelOrder(OrderId order_id);

    std::optional<Price> bestBid() const;
    std::optional<Price> bestAsk() const;

    bool empty() const;

private:
    using OrderList = std::list<Order>;

    std::map<Price, OrderList, std::greater<Price>> bids_;
    std::map<Price, OrderList> asks_;

    struct OrderLocation {
        Side side;
        Price price;
        OrderList::iterator iterator;
    };

    std::unordered_map<OrderId, OrderLocation> order_lookup_;

    void validateOrder(const Order& order) const;

    std::vector<Trade> matchBuyOrder(Order incoming);
    std::vector<Trade> matchSellOrder(Order incoming);

    void addRestingOrder(const Order& order);
};

} // namespace engine
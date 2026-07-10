#include "engine/order_book.hpp"

#include <algorithm>

#include "engine/exceptions.hpp"

namespace engine {

std::vector<Trade> OrderBook::addOrder(const Order& order)
{
    validateOrder(order);

    if (order.type == OrderType::Market) {
        if (order.side == Side::Buy) {
            return matchBuyOrder(order);
        }

        return matchSellOrder(order);
    }

    if (order.side == Side::Buy) {
        return matchBuyOrder(order);
    }

    return matchSellOrder(order);
}

bool OrderBook::cancelOrder(OrderId order_id)
{
    auto it = order_lookup_.find(order_id);

    if (it == order_lookup_.end()) {
        return false;
    }

    const auto& location = it->second;

    if (location.side == Side::Buy) {
        auto book_it = bids_.find(location.price);
        if (book_it != bids_.end()) {
            book_it->second.erase(location.iterator);

            if (book_it->second.empty()) {
                bids_.erase(book_it);
            }
        }
    } else {
        auto book_it = asks_.find(location.price);
        if (book_it != asks_.end()) {
            book_it->second.erase(location.iterator);

            if (book_it->second.empty()) {
                asks_.erase(book_it);
            }
        }
    }

    order_lookup_.erase(it);
    return true;
}

std::optional<Price> OrderBook::bestBid() const
{
    if (bids_.empty()) {
        return std::nullopt;
    }

    return bids_.begin()->first;
}

std::optional<Price> OrderBook::bestAsk() const
{
    if (asks_.empty()) {
        return std::nullopt;
    }

    return asks_.begin()->first;
}

bool OrderBook::empty() const
{
    return bids_.empty() && asks_.empty();
}

void OrderBook::validateOrder(const Order& order) const
{
    if (order.id == 0) {
        throw InvalidOrder("Order ID must be greater than zero.");
    }

    if (order.quantity == 0) {
        throw InvalidOrder("Order quantity must be greater than zero.");
    }

    if (order.type == OrderType::Limit && order.price <= 0) {
        throw InvalidOrder("Limit order price must be greater than zero.");
    }

    if (order_lookup_.contains(order.id)) {
        throw DuplicateOrderId("A resting order with this ID already exists.");
    }
}

std::vector<Trade> OrderBook::matchBuyOrder(Order incoming)
{
    std::vector<Trade> trades;

    while (incoming.remaining_quantity > 0 && !asks_.empty()) {
        auto best_ask_it = asks_.begin();
        Price best_ask_price = best_ask_it->first;

        if (incoming.type == OrderType::Limit && incoming.price < best_ask_price) {
            break;
        }

        auto& orders_at_level = best_ask_it->second;

        while (incoming.remaining_quantity > 0 && !orders_at_level.empty()) {
            auto resting_it = orders_at_level.begin();

            Quantity trade_quantity = std::min(
                incoming.remaining_quantity,
                resting_it->remaining_quantity
            );

            trades.push_back(Trade{
                incoming.id,
                resting_it->id,
                best_ask_price,
                trade_quantity,
                incoming.timestamp
            });

            incoming.remaining_quantity -= trade_quantity;
            resting_it->remaining_quantity -= trade_quantity;

            if (resting_it->remaining_quantity == 0) {
                order_lookup_.erase(resting_it->id);
                orders_at_level.erase(resting_it);
            }
        }

        if (orders_at_level.empty()) {
            asks_.erase(best_ask_it);
        }
    }

    if (
        incoming.type == OrderType::Limit &&
        incoming.remaining_quantity > 0
    ) {
        addRestingOrder(incoming);
    }

    return trades;
}

std::vector<Trade> OrderBook::matchSellOrder(Order incoming)
{
    std::vector<Trade> trades;

    while (incoming.remaining_quantity > 0 && !bids_.empty()) {
        auto best_bid_it = bids_.begin();
        Price best_bid_price = best_bid_it->first;

        if (incoming.type == OrderType::Limit && incoming.price > best_bid_price) {
            break;
        }

        auto& orders_at_level = best_bid_it->second;

        while (incoming.remaining_quantity > 0 && !orders_at_level.empty()) {
            auto resting_it = orders_at_level.begin();

            Quantity trade_quantity = std::min(
                incoming.remaining_quantity,
                resting_it->remaining_quantity
            );

            trades.push_back(Trade{
                resting_it->id,
                incoming.id,
                best_bid_price,
                trade_quantity,
                incoming.timestamp
            });

            incoming.remaining_quantity -= trade_quantity;
            resting_it->remaining_quantity -= trade_quantity;

            if (resting_it->remaining_quantity == 0) {
                order_lookup_.erase(resting_it->id);
                orders_at_level.erase(resting_it);
            }
        }

        if (orders_at_level.empty()) {
            bids_.erase(best_bid_it);
        }
    }

    if (
        incoming.type == OrderType::Limit &&
        incoming.remaining_quantity > 0
    ) {
        addRestingOrder(incoming);
    }

    return trades;
}

void OrderBook::addRestingOrder(const Order& order)
{
    if (order.side == Side::Buy) {
        auto& orders_at_level = bids_[order.price];
        orders_at_level.push_back(order);

        auto it = std::prev(orders_at_level.end());

        order_lookup_[order.id] = OrderLocation{
            order.side,
            order.price,
            it
        };
    } else {
        auto& orders_at_level = asks_[order.price];
        orders_at_level.push_back(order);

        auto it = std::prev(orders_at_level.end());

        order_lookup_[order.id] = OrderLocation{
            order.side,
            order.price,
            it
        };
    }
}

} // namespace engine
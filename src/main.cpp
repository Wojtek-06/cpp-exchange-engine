#include "engine/order_book.hpp"

#include <iostream>
#include <string_view>

namespace {

void printTrades(const std::vector<engine::Trade>& trades)
{
    if (trades.empty()) {
        std::cout << "No trades generated.\n";
        return;
    }

    for (const auto& trade : trades) {
        std::cout
            << "Trade: buy_order=" << trade.buy_order_id
            << ", sell_order=" << trade.sell_order_id
            << ", price=" << trade.price
            << ", quantity=" << trade.quantity
            << ", timestamp=" << trade.timestamp
            << '\n';
    }
}

void printTopOfBook(const engine::OrderBook& book)
{
    std::cout << "Best bid: ";

    if (const auto bid = book.bestBid()) {
        std::cout << *bid;
    } else {
        std::cout << "none";
    }

    std::cout << " | Best ask: ";

    if (const auto ask = book.bestAsk()) {
        std::cout << *ask;
    } else {
        std::cout << "none";
    }

    std::cout << "\n\n";
}

} // namespace

int main()
{
    using namespace engine;

    OrderBook book;

    std::cout << "=== C++ Exchange Engine Demo ===\n\n";

    std::cout << "Adding sell limit order: 100 units at 10100\n";
    printTrades(book.addOrder(Order{
        1,
        Side::Sell,
        OrderType::Limit,
        10100,
        100,
        1
    }));

    std::cout << "Adding sell limit order: 50 units at 10100\n";
    printTrades(book.addOrder(Order{
        2,
        Side::Sell,
        OrderType::Limit,
        10100,
        50,
        2
    }));

    std::cout << "Adding buy limit order: 80 units at 10000\n";
    printTrades(book.addOrder(Order{
        3,
        Side::Buy,
        OrderType::Limit,
        10000,
        80,
        3
    }));

    printTopOfBook(book);

    std::cout << "Adding aggressive buy limit order: 120 units at 10100\n";
    printTrades(book.addOrder(Order{
        4,
        Side::Buy,
        OrderType::Limit,
        10100,
        120,
        4
    }));

    printTopOfBook(book);

    std::cout << "Adding market sell order: 30 units\n";
    printTrades(book.addOrder(Order{
        5,
        Side::Sell,
        OrderType::Market,
        0,
        30,
        5
    }));

    printTopOfBook(book);

    std::cout << "Cancelling order 3\n";

    const bool cancelled = book.cancelOrder(3);

    std::cout
        << (cancelled ? "Cancellation successful.\n" : "Order not found.\n");

    printTopOfBook(book);

    return 0;
}
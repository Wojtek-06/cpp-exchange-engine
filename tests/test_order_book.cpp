#include "engine/order_book.hpp"

#include <gtest/gtest.h>

namespace {

using engine::Order;
using engine::OrderBook;
using engine::OrderType;
using engine::Side;

Order makeLimitOrder(
    engine::OrderId id,
    Side side,
    engine::Price price,
    engine::Quantity quantity,
    engine::Timestamp timestamp
)
{
    return Order{
        id,
        side,
        OrderType::Limit,
        price,
        quantity,
        timestamp
    };
}

Order makeMarketOrder(
    engine::OrderId id,
    Side side,
    engine::Quantity quantity,
    engine::Timestamp timestamp
)
{
    return Order{
        id,
        side,
        OrderType::Market,
        0,
        quantity,
        timestamp
    };
}

TEST(OrderBookTest, EmptyBookHasNoBestPrices)
{
    OrderBook book;

    EXPECT_TRUE(book.empty());
    EXPECT_FALSE(book.bestBid().has_value());
    EXPECT_FALSE(book.bestAsk().has_value());
}

TEST(OrderBookTest, NonCrossingLimitOrdersRestInBook)
{
    OrderBook book;

    const auto buy_trades =
        book.addOrder(makeLimitOrder(1, Side::Buy, 10000, 50, 1));

    const auto sell_trades =
        book.addOrder(makeLimitOrder(2, Side::Sell, 10100, 40, 2));

    EXPECT_TRUE(buy_trades.empty());
    EXPECT_TRUE(sell_trades.empty());

    ASSERT_TRUE(book.bestBid().has_value());
    ASSERT_TRUE(book.bestAsk().has_value());

    EXPECT_EQ(*book.bestBid(), 10000);
    EXPECT_EQ(*book.bestAsk(), 10100);
}

TEST(OrderBookTest, CrossingLimitOrdersGenerateTrade)
{
    OrderBook book;

    book.addOrder(makeLimitOrder(1, Side::Sell, 10100, 50, 1));

    const auto trades =
        book.addOrder(makeLimitOrder(2, Side::Buy, 10100, 50, 2));

    ASSERT_EQ(trades.size(), 1);

    EXPECT_EQ(trades[0].buy_order_id, 2);
    EXPECT_EQ(trades[0].sell_order_id, 1);
    EXPECT_EQ(trades[0].price, 10100);
    EXPECT_EQ(trades[0].quantity, 50);

    EXPECT_TRUE(book.empty());
}

TEST(OrderBookTest, PartialFillLeavesRemainingOrder)
{
    OrderBook book;

    book.addOrder(makeLimitOrder(1, Side::Sell, 10100, 100, 1));

    const auto trades =
        book.addOrder(makeLimitOrder(2, Side::Buy, 10100, 40, 2));

    ASSERT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].quantity, 40);

    ASSERT_TRUE(book.bestAsk().has_value());
    EXPECT_EQ(*book.bestAsk(), 10100);
    EXPECT_FALSE(book.bestBid().has_value());
}

TEST(OrderBookTest, IncomingOrderCanMatchMultipleOrders)
{
    OrderBook book;

    book.addOrder(makeLimitOrder(1, Side::Sell, 10100, 50, 1));
    book.addOrder(makeLimitOrder(2, Side::Sell, 10100, 40, 2));

    const auto trades =
        book.addOrder(makeLimitOrder(3, Side::Buy, 10100, 70, 3));

    ASSERT_EQ(trades.size(), 2);

    EXPECT_EQ(trades[0].sell_order_id, 1);
    EXPECT_EQ(trades[0].quantity, 50);

    EXPECT_EQ(trades[1].sell_order_id, 2);
    EXPECT_EQ(trades[1].quantity, 20);

    ASSERT_TRUE(book.bestAsk().has_value());
    EXPECT_EQ(*book.bestAsk(), 10100);
}

TEST(OrderBookTest, OrdersAtSamePriceUseTimePriority)
{
    OrderBook book;

    book.addOrder(makeLimitOrder(10, Side::Sell, 10100, 25, 1));
    book.addOrder(makeLimitOrder(11, Side::Sell, 10100, 25, 2));

    const auto trades =
        book.addOrder(makeLimitOrder(12, Side::Buy, 10100, 25, 3));

    ASSERT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].sell_order_id, 10);
}

TEST(OrderBookTest, BetterPriceHasPriority)
{
    OrderBook book;

    book.addOrder(makeLimitOrder(1, Side::Sell, 10200, 20, 1));
    book.addOrder(makeLimitOrder(2, Side::Sell, 10100, 20, 2));

    const auto trades =
        book.addOrder(makeLimitOrder(3, Side::Buy, 10200, 20, 3));

    ASSERT_EQ(trades.size(), 1);

    EXPECT_EQ(trades[0].sell_order_id, 2);
    EXPECT_EQ(trades[0].price, 10100);
}

TEST(OrderBookTest, MarketOrderConsumesAvailableLiquidity)
{
    OrderBook book;

    book.addOrder(makeLimitOrder(1, Side::Buy, 10000, 30, 1));

    const auto trades =
        book.addOrder(makeMarketOrder(2, Side::Sell, 20, 2));

    ASSERT_EQ(trades.size(), 1);

    EXPECT_EQ(trades[0].buy_order_id, 1);
    EXPECT_EQ(trades[0].sell_order_id, 2);
    EXPECT_EQ(trades[0].quantity, 20);

    ASSERT_TRUE(book.bestBid().has_value());
    EXPECT_EQ(*book.bestBid(), 10000);
}

TEST(OrderBookTest, UnfilledMarketOrderDoesNotRest)
{
    OrderBook book;

    const auto trades =
        book.addOrder(makeMarketOrder(1, Side::Buy, 100, 1));

    EXPECT_TRUE(trades.empty());
    EXPECT_TRUE(book.empty());
}

TEST(OrderBookTest, RestingOrderCanBeCancelled)
{
    OrderBook book;

    book.addOrder(makeLimitOrder(1, Side::Buy, 10000, 50, 1));

    EXPECT_TRUE(book.cancelOrder(1));
    EXPECT_TRUE(book.empty());
}

TEST(OrderBookTest, CancellingUnknownOrderReturnsFalse)
{
    OrderBook book;

    EXPECT_FALSE(book.cancelOrder(999));
}

} // namespace
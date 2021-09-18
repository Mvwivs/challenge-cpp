
#pragma once

#include <cstdint>
#include <deque>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

using OrderId = std::uint64_t;
using Volume = std::uint64_t;
using Price = std::int64_t; // can be negative (e.g. for swaps)

enum class Side : std::uint8_t {
    Buy, Sell
};

// Order to buy or sell specific quantity
struct Order {
    Price price;    // order price
    Volume volume;  // order quantity
    Side side;      // buy or sell side
    OrderId id;     // unique id
};

// Total volumes, aggregated by price
struct SnapshotL2 {
    using Level = std::pair<Price, Volume>;
    using Side = std::vector<Level>;
    Side asks;
    Side bids;

    bool operator==(const SnapshotL2& rhs) const;
};


using SnapshotL3 = std::vector<Order>;

// Stores and matches orders
class OrderBook {
public:
	OrderBook() = default;
	~OrderBook() = default;

    // places, modifies (if exists) or cancels order (if volume = 0)
	void update_order(const Order& order);

    // get price of top bid or ask, optionally can skip specified volume
	Price get_top(Side side, Volume skip_volume = 0) const;

    // load orderbook from orders snapshot
	void update_orderbook(const SnapshotL3& snapshot);

    // get order volumes aggregated by price
	SnapshotL2 get_snapshot() const;

private:
    // modify existing order
	void modify_order(const Order& order);

    // cancel previosly placed order
	void cancel_order(OrderId order_id);

    // place new order
	void place_order(Order order);

private:
    // one price level in order book
    struct OrderBookLevel {
        Volume total_volume;                // total volume on this price level
        std::deque<OrderId> order_queue;    // order queue, front orders are matched first
    };
    using OrderBookSide = std::map<Price, OrderBookLevel>;

    // all currently existing in orderbook orders
    std::unordered_map<OrderId, Order> orders_;

    // ask queues by price
    OrderBookSide asks_;
    
    // bid queues by price
    OrderBookSide bids_;

};

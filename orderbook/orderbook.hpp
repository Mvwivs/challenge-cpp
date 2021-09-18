
#pragma once

#include <cstdint>
#include <deque>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

using OrderId = std::uint64_t;
using Volume = std::uint64_t;
using Price = std::int64_t;

enum class Side : std::uint8_t {
    Buy, Sell
};

struct Order {
    Price price;
    Volume volume;
    Side side;
    OrderId id;
};

struct SnapshotL2 {
    using Level = std::pair<Price, Volume>;
    using Side = std::vector<Level>;
    Side asks;
    Side bids;

    bool operator==(const SnapshotL2& rhs) const;
};

using SnapshotL3 = std::vector<Order>;

class OrderBook {
public:
	OrderBook() = default;
	~OrderBook() = default;

	void update_order(const Order& order);

	Price get_top(Side side, Volume skip_volume = 0) const;

	void update_orderbook(const SnapshotL3& snapshot);

	SnapshotL2 get_snapshot() const;

private:
	void modify_order(const Order& order);

	void cancel_order(OrderId order_id);

	void place_order(Order order);

private:
    struct OrderBookLevel {
        Volume total_volume;
        std::deque<OrderId> order_queue;
    };
    using OrderBookSide = std::map<Price, OrderBookLevel>;

    std::unordered_map<OrderId, Order> orders_;
    OrderBookSide asks_;
    OrderBookSide bids_;

};

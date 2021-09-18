
#include "orderbook.hpp"

#include <algorithm>
#include <stdexcept>

bool SnapshotL2::operator==(const SnapshotL2& rhs) const {
    return ((asks == rhs.asks)
        && (bids == rhs.bids));
}

void OrderBook::update_order(const Order& order) {
    if (order.volume == 0) {
        cancel_order(order.id);
        return;
    }
    if (orders_.find(order.id) != orders_.end()) {
        modify_order(order);
        return;
    }
    place_order(order);
}

Price OrderBook::get_top(Side side, Volume skip_volume) const {
    Volume skipped = 0;
    if (side == Side::Sell) {
        for (auto it = asks_.begin(); it != asks_.end(); ++it) {
            const auto& [best, best_level] = *it;
            skipped += best_level.total_volume;
            if (skipped > skip_volume) {
                return best;
            }
        }
    }
    else {
        for (auto it = bids_.rbegin(); it != bids_.rend(); ++it) {
            const auto& [best, best_level] = *it;
            skipped += best_level.total_volume;
            if (skipped > skip_volume) {
                return best;
            }
        }
    }
    throw std::runtime_error("Not enough volume in orderbook to skip");
}

void OrderBook::update_orderbook(const SnapshotL3& snapshot) {
    for (const auto& order : snapshot) {
        update_order(order);
    }
}

SnapshotL2 OrderBook::get_snapshot() const {
    SnapshotL2 snapshot;
    snapshot.asks.reserve(asks_.size());
    for (const auto& [price, level] : asks_) {
        snapshot.asks.push_back({price, level.total_volume});
    }
    snapshot.bids.reserve(bids_.size());
    for (const auto& [price, level] : bids_) {
        snapshot.bids.push_back({price, level.total_volume});
    }
    return snapshot;
}

void OrderBook::modify_order(const Order& order) {
    cancel_order(order.id);
    place_order(order);
}

void OrderBook::cancel_order(OrderId order_id) {
    const auto it = orders_.find(order_id);
    if (it == orders_.end()) {
        throw std::runtime_error("No order with id: " + std::to_string(order_id));
    }

    const auto& [_id, order] = *it;
    OrderBookSide& side = (order.side == Side::Sell) ? asks_ : bids_;
    OrderBookLevel& level = side[order.price];
    level.total_volume -= order.volume;
    level.order_queue.erase(std::find(level.order_queue.begin(), level.order_queue.end(), order_id));
    if (level.total_volume == 0) {
        side.erase(order.price); // cleanup empty levels
    }
    orders_.erase(it);
}

void OrderBook::place_order(Order order) {
    OrderBookSide& side = (order.side == Side::Sell) ? asks_ : bids_;
    OrderBookSide& opposite_side = (order.side == Side::Buy) ? asks_ : bids_;
    while (!opposite_side.empty() && (order.volume != 0)) {
        auto& [best_price, best] = (order.side == Side::Buy) ? 
            *(opposite_side.begin()) : *(--opposite_side.end());
        if (((best_price < order.price) && (order.side == Side::Sell))
            || ((best_price > order.price) && (order.side == Side::Buy))) {
            break; // ob is not crossing
        }

        // match, other side has orders at this or better price
        while ((best.total_volume != 0) && (order.volume != 0)) {
            OrderId matching_id = best.order_queue.front();
            Order& matching_order = orders_[matching_id];
            if (matching_order.volume <= order.volume) { // full execution
                order.volume -= matching_order.volume;
                best.total_volume -= matching_order.volume;
                best.order_queue.pop_front();
                orders_.erase(matching_id);
            }
            else { // partial execution
                matching_order.volume -= order.volume;
                best.total_volume -= order.volume;
                order.volume = 0;
            }
        }
        // TODO: create trade event

        if (best.total_volume == 0) { // cleanup empty levels
            if (order.side == Side::Buy) {
                opposite_side.erase(opposite_side.begin());
            }
            else {
                opposite_side.erase(--opposite_side.end());
            }
        }
    }

    if (order.volume != 0) {
        OrderBookLevel& level = side[order.price];
        level.total_volume += order.volume;
        level.order_queue.push_back(order.id);
        orders_[order.id] = order;
    }
}

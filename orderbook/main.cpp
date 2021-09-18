
#include <iostream>
#include <cassert>

#include "orderbook.hpp"

void test_empty() {
    OrderBook ob;
    SnapshotL2 snapshot = ob.get_snapshot();
    SnapshotL2 expected = {{}, {}};
    assert(snapshot == expected);
}

void test_place_one_bid() {
    OrderBook ob;
    ob.update_order({10, 10, Side::Buy, 0});
    SnapshotL2 snapshot = ob.get_snapshot();
    SnapshotL2 expected = {
        {},
        {{10, 10}}
    };
    assert(snapshot == expected);
}

void test_place_one_ask() {
    OrderBook ob;
    ob.update_order({10, 10, Side::Sell, 0});
    SnapshotL2 snapshot = ob.get_snapshot();
    SnapshotL2 expected = {
        {{10, 10}},
        {}
    };
    assert(snapshot == expected);
}

void test_place_several() {
    OrderBook ob;
    ob.update_order({300, 10, Side::Sell, 0});
    ob.update_order({400, 5, Side::Sell, 1});
    ob.update_order({100, 3, Side::Buy, 2});
    ob.update_order({200, 2, Side::Buy, 3});
    SnapshotL2 snapshot = ob.get_snapshot();
    SnapshotL2 expected = {
        {{300, 10}, {400, 5}},
        {{100, 3}, {200, 2}}
    };
    assert(snapshot == expected);
}

void test_aggregate() {
    OrderBook ob;
    ob.update_order({200, 10, Side::Sell, 0});
    ob.update_order({200, 5, Side::Sell, 1});
    ob.update_order({100, 3, Side::Buy, 2});
    ob.update_order({100, 2, Side::Buy, 3});
    SnapshotL2 snapshot = ob.get_snapshot();
    SnapshotL2 expected = {
        {{200, 15}},
        {{100, 5}}
    };
    assert(snapshot == expected);
}

void test_match_full_empty() {
    OrderBook ob;
    ob.update_order({200, 10, Side::Sell, 0});
    ob.update_order({200, 10, Side::Buy, 1});
    SnapshotL2 snapshot = ob.get_snapshot();
    SnapshotL2 expected = {{}, {}};
    assert(snapshot == expected);
}

void test_match_full_placed() {
    OrderBook ob;
    ob.update_order({200, 5, Side::Sell, 0});
    ob.update_order({200, 10, Side::Buy, 1});
    SnapshotL2 snapshot = ob.get_snapshot();
    SnapshotL2 expected = {
        {},
        {{200, 5}}
    };
    assert(snapshot == expected);
}

void test_match_partial() {
    OrderBook ob;
    ob.update_order({200, 10, Side::Sell, 0});
    ob.update_order({200, 5, Side::Buy, 1});
    SnapshotL2 snapshot = ob.get_snapshot();
    SnapshotL2 expected = {
        {{200, 5}},
        {}
    };
    assert(snapshot == expected);
}

void test_cross_full_empty() {
    OrderBook ob;
    ob.update_order({100, 10, Side::Sell, 0});
    ob.update_order({200, 10, Side::Buy, 1});
    SnapshotL2 snapshot = ob.get_snapshot();
    SnapshotL2 expected = {{}, {}};
    assert(snapshot == expected);
}

void test_cross_full_placed() {
    OrderBook ob;
    ob.update_order({100, 5, Side::Sell, 0});
    ob.update_order({200, 10, Side::Buy, 1});
    SnapshotL2 snapshot = ob.get_snapshot();
    SnapshotL2 expected = {
        {},
        {{200, 5}}
    };
    assert(snapshot == expected);
}

void test_cross_partial() {
    OrderBook ob;
    ob.update_order({100, 10, Side::Sell, 0});
    ob.update_order({200, 5, Side::Buy, 1});
    SnapshotL2 snapshot = ob.get_snapshot();
    SnapshotL2 expected = {
        {{100, 5}},
        {}
    };
    assert(snapshot == expected);
}

void test_cross_deep_bid() {
    OrderBook ob;
    ob.update_order({400, 10, Side::Sell, 0});
    ob.update_order({300, 10, Side::Sell, 1});
    ob.update_order({200, 10, Side::Sell, 2});
    ob.update_order({100, 10, Side::Sell, 3});
    ob.update_order({350, 30, Side::Buy, 4});
    SnapshotL2 snapshot = ob.get_snapshot();
    SnapshotL2 expected = {
        {{400, 10}},
        {}
    };
    assert(snapshot == expected);
}

void test_cross_deep_ask() {
    OrderBook ob;
    ob.update_order({300, 10, Side::Buy, 0});
    ob.update_order({200, 10, Side::Buy, 1});
    ob.update_order({100, 10, Side::Buy, 2});
    ob.update_order({50, 10, Side::Buy, 3});
    ob.update_order({50, 30, Side::Sell, 4});
    SnapshotL2 snapshot = ob.get_snapshot();
    SnapshotL2 expected = {
        {},
        {{50, 10}}
    };
    assert(snapshot == expected);
}

void test_cancel_bid() {
    OrderBook ob;
    ob.update_order({10, 10, Side::Buy, 0});
    ob.update_order({10, 0, Side::Buy, 0});
    SnapshotL2 snapshot = ob.get_snapshot();
    SnapshotL2 expected = {{},{}};
    assert(snapshot == expected);
}

void test_cancel_ask() {
    OrderBook ob;
    ob.update_order({10, 10, Side::Sell, 0});
    ob.update_order({10, 0, Side::Sell, 0});
    SnapshotL2 snapshot = ob.get_snapshot();
    SnapshotL2 expected = {{},{}};
    assert(snapshot == expected);
}

void test_cancel_empty() {
    OrderBook ob;
    try {
        ob.update_order({10, 0, Side::Sell, 0});
    }
    catch (const std::runtime_error& e) {
        return;
    }
    assert(false && "Exception thrown");
}

void test_cancel_queue() {
    OrderBook ob;
    ob.update_order({10, 10, Side::Sell, 0});
    ob.update_order({10, 10, Side::Sell, 1});
    ob.update_order({10, 10, Side::Sell, 2});

    ob.update_order({10, 0, Side::Sell, 1});
    SnapshotL2 snapshot = ob.get_snapshot();
    SnapshotL2 expected = {
        {{10, 20}},
        {}
    };
    assert(snapshot == expected);
}

void test_update_orderbook() {
    OrderBook ob;
    ob.update_orderbook({
        {10, 30, Side::Sell, 0},
        {5, 5, Side::Buy, 1},
        {20, 8, Side::Sell, 2},
        {1, 8, Side::Buy, 3},
    });

    SnapshotL2 snapshot = ob.get_snapshot();
    SnapshotL2 expected = {
        {{10, 30}, {20, 8}},
        {{1, 8}, {5, 5}}
    };
    assert(snapshot == expected);
}

void test_modify() {
    OrderBook ob;
    ob.update_order({300, 10, Side::Sell, 0});
    ob.update_order({200, 10, Side::Sell, 1});
    ob.update_order({100, 10, Side::Buy, 2});
    ob.update_order({50, 10, Side::Buy, 3});

    ob.update_order({250, 15, Side::Buy, 2});
    SnapshotL2 snapshot = ob.get_snapshot();
    SnapshotL2 expected = {
        {{300, 10}},
        {{50, 10}, {250, 5}}
    };
    assert(snapshot == expected);
}

void test_get_top() {
    OrderBook ob;
    ob.update_order({300, 10, Side::Sell, 0});
    ob.update_order({200, 10, Side::Sell, 1});
    ob.update_order({100, 10, Side::Buy, 2});
    ob.update_order({50, 10, Side::Buy, 3});

    Price top_bid = ob.get_top(Side::Buy);
    Price top_ask = ob.get_top(Side::Sell);
    assert(top_bid == 100);
    assert(top_ask == 200);
}

void test_get_top_skip() {
    OrderBook ob;
    ob.update_order({300, 10, Side::Sell, 0});
    ob.update_order({200, 10, Side::Sell, 1});
    ob.update_order({100, 10, Side::Buy, 2});
    ob.update_order({50, 10, Side::Buy, 3});

    Price top_bid = ob.get_top(Side::Buy, 15);
    Price top_ask = ob.get_top(Side::Sell, 15);
    assert(top_bid == 50);
    assert(top_ask == 300);
}

int main() {
    test_empty();
    test_place_one_bid();
    test_place_one_ask();
    test_place_several();
    test_aggregate();

    test_match_full_empty();
    test_match_full_placed();
    test_match_partial();
    test_cross_full_empty();
    test_cross_full_placed();
    test_cross_partial();
    test_cross_deep_bid();
    test_cross_deep_ask();

    test_cancel_bid();
    test_cancel_ask();
    test_cancel_empty();
    test_cancel_queue();

    test_update_orderbook();
    test_modify();

    test_get_top();
    test_get_top_skip();

    std::cout << "Passed all tests" << std::endl;
}

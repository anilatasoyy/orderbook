#pragma once
#include <vector>
#include "orderbook/Order.h"

#ifdef ORDERBOOK_TRACE
    #include <iostream>
    #define OB_TRACE(x) do { std::cout << x; } while (0)
#else
    #define OB_TRACE(x) do { } while (0)
#endif

class Stock {
public:
    void addOrder(Order order);
    void printBook() const;
    void transaction(Order& order);

private:
    static constexpr long MIN_PRICE = 9900;
    static constexpr long MAX_PRICE = 10100;
    static constexpr int  NUM_LEVELS = MAX_PRICE - MIN_PRICE + 1;   // 201

    std::vector<Order> bid_levels[NUM_LEVELS];
    std::vector<Order> ask_levels[NUM_LEVELS];
    int price_to_index(long price) const { return (int)(price - MIN_PRICE); }
    long index_to_price(int idx) const { return (long)idx + MIN_PRICE; }
    int best_ask_idx{-1};
    int best_bid_idx{-1};
    void advance_best_ask();
    void advance_best_bid();
    bool crosses(const Order& order, int best_index) const;
};

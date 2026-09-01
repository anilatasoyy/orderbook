#pragma once
#include <vector>
#include "orderbook/Order.h"
#include <cstdint>

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
    static constexpr long MIN_PRICE = 5000;
    static constexpr long MAX_PRICE = 15000;
    static constexpr int  NUM_LEVELS = MAX_PRICE - MIN_PRICE + 1;   // 10001
    static constexpr int BITS_PER_WORD = 64;
    static constexpr int NUM_WORDS = (NUM_LEVELS + BITS_PER_WORD - 1) / BITS_PER_WORD; // rounding error fix

    uint64_t bid_occupancy[NUM_WORDS] = {0};
    uint64_t ask_occupancy[NUM_WORDS] = {0};

    struct Level {
        std::vector<Order> orders;
        int head = 0;
        static constexpr int RESERVE_SIZE = 16;

        Level() { orders.reserve(RESERVE_SIZE); }

        bool empty() const { return head >= (int)orders.size(); }
        Order& front() { return orders[head]; }
        void pop_front() { ++head; }
        void push(const Order& o) {orders.push_back(o); }
        void reset() {orders.clear(); head = 0;}
    };

    Level bid_levels[NUM_LEVELS];
    Level ask_levels[NUM_LEVELS];
    int price_to_index(long price) const { return (int)(price - MIN_PRICE); }
    long index_to_price(int idx) const { return (long)idx + MIN_PRICE; }
    int best_ask_idx{-1};
    int best_bid_idx{-1};
    void advance_best_ask();
    void advance_best_bid();
    bool crosses(const Order& order, int best_index) const;
    void set_bit(uint64_t* bitset, int idx) { bitset[idx / 64] |= (1ULL << (idx % 64)); }
    void clear_bit(uint64_t* bitset, int idx) {bitset[idx / 64] &= ~(1ULL << (idx % 64)); }
};

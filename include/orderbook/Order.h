#pragma once

struct Order {
    int id;
    bool is_buy;
    long price;      // ticks (cents), integer
    int quantity;

    Order(int id, bool is_buy, long price, int quantity);
};

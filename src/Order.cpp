#include "orderbook/Order.h"

Order::Order(int id, bool is_buy, long price, int quantity)
    : id(id), is_buy(is_buy), price(price), quantity(quantity) {}

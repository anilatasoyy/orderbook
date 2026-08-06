#pragma once
#include <map>
#include <vector>
#include "orderbook/Order.h"

class Stock {
public:
    void addOrder(Order order);
    void printBook() const;
    void transaction(Order& order);

private:
    std::map<long, std::vector<Order>> bids;
    std::map<long, std::vector<Order>> asks;
    bool crosses(const Order &order,const std::map<long, std::vector<Order>>& book) const;
};

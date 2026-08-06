#include "orderbook/Stock.h"

int main()
{
    Stock stock;

    // buy test
    stock.addOrder(Order(0, false, 1100, 24));
    stock.addOrder(Order(1, false, 1100, 20));
    stock.addOrder(Order(2, true, 1100, 30)); // TRADE 24, TRADE 6, asks 1100 x 14
    // then sell test in a fresh Stock
    stock.addOrder(Order(0, true, 1000, 50));
    stock.addOrder(Order(1, false, 1000, 30)); // TRADE 30, bids 1000 x 20
    stock.printBook();
    return 0;
}

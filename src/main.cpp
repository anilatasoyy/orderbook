#include "orderbook/Stock.h"

int main()
{
    Stock stock;

    // --- buy-side matching test ---
    stock.addOrder(Order(0, false, 10000, 24));   // rest ask 24 @ 10000
    stock.addOrder(Order(1, false, 10000, 20));   // rest ask 20 @ 10000 (same level)
    stock.addOrder(Order(2, true,  10000, 30));   // buy 30 -> TRADE 24, TRADE 6, asks left 14

    // --- sell-side matching test ---
    stock.addOrder(Order(3, true,  9950, 50));    // rest bid 50 @ 9950
    stock.addOrder(Order(4, false, 9950, 30));    // sell 30 -> TRADE 30, bid left 20

    stock.printBook();
    return 0;
}

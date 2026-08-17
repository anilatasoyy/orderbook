#include "orderbook/Stock.h"

int main()
{
    Stock stock;

    stock.addOrder(Order(0, false, 10000, 24));
    stock.addOrder(Order(1, false, 10000, 20));
    stock.addOrder(Order(2, true, 10000, 30));
    stock.addOrder(Order(3, true, 9950, 50));
    stock.addOrder(Order(4, false, 9950, 30));
    stock.printBook();
    return 0;
}

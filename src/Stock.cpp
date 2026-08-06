#include <iostream>
#include "orderbook/Stock.h"

void Stock::addOrder(Order order)
{
    transaction(order);
    if (order.quantity > 0)
    {
        if (order.is_buy)
            bids[order.price].push_back(order);
        else
            asks[order.price].push_back(order);
    }
}

void Stock::printBook() const
{
    std::cout << "ASKS\n";
    for (const auto &level : asks)
    {
        int total = 0;
        for (const auto &ord : level.second)
            total += ord.quantity;
        std::cout << level.first << " x " << total << "\n";
    }
    std::cout << "-------------------\n";
    std::cout << "BIDS\n";
    for (const auto &level : bids)
    {
        int total = 0;
        for (const auto &ord : level.second)
            total += ord.quantity;
        std::cout << level.first << " x " << total << "\n";
    }
}

// Does the incoming order cross the book's best price?
// Buy crosses when its price >= lowest ask; sell crosses when its price <= highest bid.
bool Stock::crosses(const Order &order,const std::map<long, std::vector<Order>>& book) const {

    if(book.empty()) return false;
    if(order.is_buy) return order.price >= book.begin()->first;
    else             return order.price <= std::prev(book.end())->first;

}

void Stock::transaction(Order &order)
{
    // Match against the opposite book: a buy consumes asks, a sell consumes bids.
    std::map<long, std::vector<Order>>& book = order.is_buy ? asks : bids;

   
        while (order.quantity > 0 && crosses(order, book))   // Keep matching while the incoming order has quantity left and still crosses the book.
        { 
             // Best level: lowest ask for a buy (begin), highest bid for a sell (prev(end)).
            auto best = order.is_buy ? book.begin() : std::prev(book.end());
            auto &resting = best->second.front();   // earliest order at that price (time priority)
            long price = best->first;               // trades execute at the resting order's price





            if (resting.quantity > order.quantity)
            {   // Resting order is larger: incoming is fully filled, resting stays (reduced).
                std::cout << "TRADE: " << order.quantity << " @ " << price << "\n";
                resting.quantity -= order.quantity;
                order.quantity = 0;
            }
            
            else
            { 
                // Resting order is smaller or equal: it's fully consumed and removed;
                // incoming keeps whatever's left over and continues to the next level.
                std::cout << "TRADE: " << resting.quantity << " @ " << price << "\n";
                order.quantity -= resting.quantity;
                best->second.erase(best->second.begin()); // remove the filled resting order
                if (best->second.empty()) book.erase(best); // drop the level if now empty
            }
        }

}

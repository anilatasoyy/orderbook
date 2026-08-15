#include <iostream>
#include "orderbook/Stock.h"

void Stock::addOrder(Order order)
{
    transaction(order);
    if (order.quantity > 0)
    {
        if (order.is_buy)
            bid_levels[price_to_index(order.price)].push_back(order);
        else
            ask_levels[price_to_index(order.price)].push_back(order);
    }
}

void Stock::printBook() const
{
    std::cout << "ASKS\n";
    
    for (int i = 0; i<NUM_LEVELS; i++)
    {
        if (ask_levels[i].empty()) continue;  
        int total = 0;
        for (const auto &ord : ask_levels[i])
            total += ord.quantity;
        std::cout << index_to_price(i) << " x " << total << "\n";
    }
    std::cout << "-------------------\n";
    std::cout << "BIDS\n";
    for (int i = 0; i<NUM_LEVELS; i++)
    {
        if (bid_levels[i].empty()) continue;  
        int total = 0;
        for (const auto &ord : bid_levels[i])
            total += ord.quantity;
        std::cout << index_to_price(i) << " x " << total << "\n";
    }
}

int Stock::best_ask_index() const {
    for (int i = 0; i < NUM_LEVELS; ++i)
        if (!ask_levels[i].empty()) return i;
    return -1;   // no asks resting
}

int Stock::best_bid_index() const {
    for (int i = NUM_LEVELS - 1; i >= 0; --i)
        if (!bid_levels[i].empty()) return i;
    return -1;   // no bids resting
}

// Does the incoming order cross the book's best price?
// Buy crosses when its price >= lowest ask; sell crosses when its price <= highest bid.
bool Stock::crosses(const Order &order,const int best_index) const {

    if(best_index == -1) return false;
    long price = index_to_price(best_index);
    return order.is_buy ? order.price >= price : order.price <= price;

}

void Stock::transaction(Order &order)
{
    // Match against the opposite book: a buy consumes asks, a sell consumes bids.
   // std::map<long, std::vector<Order>>& book = order.is_buy ? asks : bids;

   
        while (order.quantity > 0)   // Keep matching while the incoming order has quantity left and still crosses the book.
        { 
            int best = order.is_buy ? best_ask_index() : best_bid_index();

            if(!crosses(order,best)) break;

            std::vector<Order>& level = order.is_buy ? ask_levels[best] : bid_levels[best];

            Order& resting = level.front();               // earliest order at that price (time priority)
            long price = index_to_price(best);           // trades execute at the resting order's price





            if (resting.quantity > order.quantity)
            {   // Resting order is larger: incoming is fully filled, resting stays (reduced).
                OB_TRACE("TRADE: " << order.quantity << " @ " << price << "\n");
                resting.quantity -= order.quantity;
                order.quantity = 0;
            }
            
            else
            { 
                // Resting order is smaller or equal: it's fully consumed and removed;
                // incoming keeps whatever's left over and continues to the next level.
                OB_TRACE("TRADE: " << resting.quantity << " @ " << price << "\n");
                order.quantity -= resting.quantity;
                level.erase(level.begin());
            }
        }

}

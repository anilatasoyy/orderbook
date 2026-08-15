#include "orderbook/Stock.h"
#include <chrono>
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>

int main()
{
    Stock stock;

    const int N = 100000; // order num

    // rng
    std::mt19937 rng(42);                                        // sabit seed
    std::uniform_int_distribution<long> price_dist(9900, 10100); // 200 aralık
    std::uniform_int_distribution<int> qty_dist(1, 100);
    std::uniform_int_distribution<int> side_dist(0, 1);

    std::vector<Order> orders;
    orders.reserve(N);

    for (int i = 0; i < N; ++i)
    {
        orders.emplace_back(i, side_dist(rng), price_dist(rng), qty_dist(rng));
    }

    // --- measure per-order latency ---
    std::vector<long long> latencies;
    latencies.reserve(N);

    // --- warmup: run orders through a throwaway book, untimed ---
    // Warms caches, allocator, branch predictor before real measurement.
    {
        Stock warmup_stock;
        for (auto &o : orders)
        {
            Order copy = o; // addOrder takes by value anyway
            warmup_stock.addOrder(copy);
        }
    }

    // order başına zaman ölçümü
    for (auto &o : orders)
    {
        auto start = std::chrono::high_resolution_clock::now();
        stock.addOrder(o);
        auto end = std::chrono::high_resolution_clock::now();
        latencies.push_back(
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    long long sum = 0;
    for (auto l : latencies)
        sum += l;

    // --- analysis: sort, then extract percentiles ---   ← THIS BLOCK WAS MISSING
    std::sort(latencies.begin(), latencies.end());
    auto pct = [&](double p)
    {
        return latencies[(size_t)(p * (latencies.size() - 1))];
    };

    std::cout << "orders:  " << N << "\n";
    std::cout << "mean:    " << (sum / N) << " ns\n";
    std::cout << "p50:     " << pct(0.50) << " ns\n";
    std::cout << "p90:     " << pct(0.90) << " ns\n";
    std::cout << "p99:     " << pct(0.99) << " ns\n";
    std::cout << "p99.9:   " << pct(0.999) << " ns\n";
    std::cout << "max:     " << latencies.back() << " ns\n";
    return 0;
}
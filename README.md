# Limit Order Book & Matching Engine (C++17)

A single-threaded limit order book with a matching engine, built from scratch in C++17 and optimized through a measured sequence of data-structure changes. Median latency for a full add-and-match operation is **~51 ns** on modern server hardware (**~107 ns** on an older desktop under WSL2).

The focus of this project is not just a working matching engine, but the **optimization arc**: each change was benchmarked, and several were kept or discarded based on measurement rather than intuition — including one deliberate regression that motivated the fix after it.

## What it does

- Maintains bids and asks for a single instrument, organized by price level.
- Enforces **price-time priority**: best price first, earliest arrival first within a price.
- On each incoming order: checks whether it crosses the opposite side, matches and fills against resting orders as far as it crosses (executing at the resting/maker price), and rests any remaining quantity.
- Prices are integer ticks (not floating point), avoiding float-comparison bugs in matching.

**Deliberately out of scope** (single-threaded matching hot path only): order cancellation, multi-instrument support, threading, kernel-bypass networking, and hardware offload. These are noted as future work below.

## The optimization arc

Each step was measured against a fixed-seed workload (see Methodology). Numbers below are per-order latency on the i5-8600K / WSL2 setup unless noted.

| Step | Change | Result | Commit |
|------|--------|--------|--------|
| 1 | Baseline: `std::map` + `std::vector` per level | p50 ~408 ns | `d20dff1` |
| 2 | Replace map with a flat array indexed by price, best-price found by linear scan | **regression** — p50 ~2780 ns | `61a17c7` |
| 3 | Track best-bid/best-ask indices (O(1) lookup instead of scanning) | p50 ~105 ns (~4× vs map) | `d4a00cf` |
| 4 | Head-index per level: O(1) front removal instead of `vector::erase` (O(n)) | p99 639 ns → tail tightened | `78ae67d` |
| 5 | Pre-reserve per-level capacity to avoid mid-run reallocation | p99.9 ~1.8× improvement | `b13d71a` |
| 6 | Widen price range to ~10k levels (scale experiment) — **deliberately exposes** the O(n) `advance_best` scan | p99 524 ns → **17,355 ns** (tail-only; median flat) | `146111e` |
| 7 | Occupancy bitset + `__builtin_ctzll`/`clzll` for best-price lookup | p99 **17,355 ns → 426 ns (~41×)** | `8164827` |

The two instructive moments:

- **Step 2 was a regression, kept in the history on purpose.** Replacing the red-black tree with an array *should* have been faster, but finding the best price by scanning a mostly-empty array was ~7× slower than the tree. Big-O intuition ("array is O(1)") missed that *finding* the best price had become O(n). This is why every subsequent change was measured, not assumed.
- **Step 6 deliberately breaks the book to justify Step 7.** Widening the price range makes the book sparse, so the linear `advance_best` scan walks thousands of empty levels — p99 explodes 33× while the median stays flat (a textbook tail-latency signature). Step 7's occupancy bitset finds the next occupied level with a hardware bit-scan (`ctz`/`clz`), skipping 64 levels per word, collapsing the tail ~41×.

## Benchmark methodology

- **Workload:** 100,000 randomly generated orders (fixed RNG seed for reproducibility), integer-tick prices over the configured range, random sizes and sides.
- **Warmup:** orders are run through a throwaway book before measurement to warm caches, allocator, and branch predictor.
- **Two measurements:** per-order latency (wrapped in `steady_clock`, sorted for percentiles) and a single batch timer for throughput. The batch average tracks closely with the per-order median, confirming the per-call clock overhead is small and the percentile numbers are trustworthy.
- **Honest caveats:** these are **warm-path, steady-state** numbers on a repetitive workload — not worst-case or cold-start. The reported `max` (tens of µs) is OS scheduler jitter, not algorithmic cost; it persists even on dedicated, pinned hardware, which is why production systems require kernel-level isolation beyond core pinning.

## Results

Final version, best of several runs:

| Metric | i5-8600K, WSL2 (unpinned) | Xeon Platinum 8488C, dedicated + pinned core |
|--------|---------------------------|----------------------------------------------|
| batch avg | 91 ns/order | **47 ns/order** |
| p50 | 107 ns | **51 ns** |
| p90 | 214 ns | 91 ns |
| p99 | 426 ns | **145 ns** |
| p99.9 | 534 ns | 205 ns |
| max | ~55 µs (OS jitter) | ~18 µs (OS jitter) |

The improvement *ratios* across the optimization arc hold across both environments, indicating the gains are algorithmic rather than artifacts of a particular machine. The absolute drop from WSL to bare metal (~2×) reflects the environment (no VM layer, pinned core, newer CPU), not code changes.

## Key design decisions

- **Flat array indexed by price, not a tree/map.** Prices are bounded integer ticks, so a price *is* an array index — O(1) access, contiguous (cache-friendly) memory, no per-node allocation or pointer-chasing.
- **Tracked best-bid/ask indices + occupancy bitset.** Finding the top of book is O(1) in the common case; when a level empties, the bitset + `ctz`/`clz` finds the next occupied level in ~O(n/64), which keeps the working set cache-resident regardless of how wide the price range is.
- **Head-index per level instead of erasing from the front.** Advancing a cursor is O(1); shifting a vector is O(n). This was the fix for a deep-level tail-latency problem.
- **Tail latency over mean.** In matching, the 99.9th percentile matters far more than the average — the slow cases cluster at exactly the busy moments that matter. The optimization work targeted the tail specifically.

## Where this sits in a real system

A production trading pipeline's tick-to-trade budget is measured in microseconds and dominated by **network I/O**, not matching (e.g. published figures put the end-to-end budget around 30 µs, most of it network). This project implements the in-memory matching component — a sub-microsecond piece of that larger pipeline — and optimizes for the property production systems actually evaluate on: worst-case (tail) latency, not average.

## Build & run

Requires a C++17 compiler and CMake.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/benchmark          # runs the latency + throughput benchmark
./build/orderbook          # demo with trade output (ORDERBOOK_TRACE enabled)
```

## Possible future work

- Order cancellation (interacts with the occupancy bitset — clearing a level's bit on last-order removal).
- Multi-instrument support (a book per symbol).
- Zero-allocation order pool + intrusive list for O(1) cancellation anywhere in a level.
- Hierarchical bitset for true O(1) best-price lookup on extremely sparse books.
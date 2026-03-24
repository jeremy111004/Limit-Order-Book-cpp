# High-Performance Limit Order Book (LOB)

This project is a C++20 matching engine designed to evolve from a **naive baseline** to a **highly optimized production-grade** system. This repository serves as a performance study, allowing users to track architectural improvements through the commit history and upcoming benchmarks.

---

## Evolution Roadmap

The engine is developed in distinct phases to isolate and analyze performance bottlenecks:

* **v1 (Baseline)**: Focuses on functional correctness using standard containers (`std::map`, `std::list`).
* **v2 (Memory Optimized)**: Integration of a custom **Memory Arena** (Bump Allocator) and **Placement New** to eliminate heap fragmentation and allocation latency.
* **v3 (Low-Latency)**: Implementation of **Intrusive Freelists**, **Cache-Line Alignment**, and **Huge Pages** to minimize TLB misses and cache churn.

---

## Current Architecture (v1)

The current implementation utilizes **Price-Time Priority (FIFO)** logic.

| Component | Responsibility | Technical Choice |
| :--- | :--- | :--- |
| **Data Model** | `Order.hpp` | POD (Plain Old Data) structs for cache efficiency. |
| **Logic** | `OrderBook.cpp` | Core matching algorithm and state management. |
| **Price Index** | `std::map` | Logarithmic ($O(\log P)$) price level sorting. |
| **Storage** | `std::list` | Stable memory addresses for $O(1)$ cancellations. |

---

## Complexity Analysis

| Operation | Complexity | Technical Justification |
| :--- | :--- | :--- |
| **Add Order** | $O(\log P)$ | Red-Black tree traversal for price level insertion. |
| **Cancel Order** | $O(1)$ | Hash map lookup followed by iterator-based erasure. |
| **Match Level** | $O(M)$ | Linear traversal of $M$ orders at the best price level. |
| **Modify Order** | $O(\log P)$ | Cancellation + Re-insertion (maintains price-time priority). |

---

## Build and Execution

### Requirements
* **Compiler**: GCC 10+ or Clang 12+ (C++20 support).
* **Environment**: Linux (recommended for performance profiling).

### Compilation
Use `-O3` and `-march=native` for maximum optimization during benchmarking:

```bash
g++ -std=c++20 -O3 -march=native -Wall -Wextra main.cpp OrderBook.cpp -o exchange

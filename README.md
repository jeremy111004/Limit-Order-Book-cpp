# Limit Order Book (LOB)

A high-performance matching engine implementation in C++20. This engine utilizes **Price-Time Priority** (FIFO) logic to reconcile trades. It is architected for low-latency operations by using stabilized iterators to achieve $O(1)$ complexity for order cancellations and modifications.

---

## Technical Architecture

The engine is partitioned into four components to maintain the **Single Responsibility Principle**:

| File | Component | Description |
| :--- | :--- | :--- |
| **`Order.hpp`** | Data Model | Plain Old Data (POD) structs for `Order` and `matchResult`. |
| **`OrderBook.hpp`** | Interface | `LOB` class declaration and private data member specifications. |
| **`OrderBook.cpp`** | Implementation | Core logic for matching algorithms and state management. |
| **`main.cpp`** | Driver | Entry point, simulation harness, and order generation. |

---

## Data Structures & Complexity

The implementation prioritizes efficient lookups and stable memory addresses:

* **Price-Time Priority**: `std::map<uint32_t, std::list<Order>>`. Maps price levels to a doubly-linked list. This ensures logarithmic price sorting and constant-time seniority tracking.
* **Order Tracking**: `std::unordered_map<uint64_t, std::list<Order>::iterator>`. Enables $O(1)$ access to any live order for immediate cancellation or modification without scanning the lists.

### Complexity Analysis
| Operation | Complexity | Technical Justification |
| :--- | :--- | :--- |
| **Add Order** | $O(\log P)$ | Red-Black tree traversal for price level insertion. |
| **Cancel Order** | $O(1)$ | Hash map lookup + pointer-based list erasure. |
| **Match Level** | $O(M)$ | Linear traversal of $M$ orders at the best price level. |
| **Edit (Price/Qty Up)** | $O(\log P)$ | Price-Time impact: requires cancellation and re-insertion. |

---

## Build and Execution

### Requirements
* **Compiler**: GCC 10+ or Clang 12+ (C++20 support required).
* **Environment**: Linux (Fedora/Ubuntu/Debian).

### Compilation
Use the following command to compile with `-O3` optimizations and strict warning flags:

```bash
g++ -std=c++20 -O3 -Wall -Wextra main.cpp OrderBook.cpp -o exchange

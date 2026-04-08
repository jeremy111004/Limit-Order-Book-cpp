#include "OrderBook.hpp"
#include <benchmark/benchmark.h>
#include <benchmark/state.h>
#include <benchmark/utils.h>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

// The Generator is kept here because the LOB should not have access
// or see how fake orders are created.
Order OrdersGenerator(uint32_t price, char type, uint64_t id) {
  int min = price - 4;
  int max = price + 4;

  static std::random_device rd;
  static std::mt19937 gen(rd());

  std::uniform_int_distribution<> distribP(min, max);
  std::uniform_int_distribution<> distribQ(1, 50);

  // C++20 Designated Initializers: Clear, safe, and warning-free
  return Order{.id = id,
               .price = static_cast<uint32_t>(distribP(gen)),
               .quantity = static_cast<uint32_t>(distribQ(gen)),
               .type = type,
               .enabled = true,
               .next = nullptr,
               .prev = nullptr};
}

static void LOB_Naive_Continuous(benchmark::State &state) {
  LOB lob;
  std::vector<Order> orders;

  // 10^5 is a robust sample size for L1/L2 cache testing.
  // 10^9 will trigger a Linux OOM (Out Of Memory) crash.
  const uint32_t num_orders = 1000000;

  orders.reserve(num_orders * 2);

  // Setup Phase: Prepare data outside the timed loop
  for (uint32_t i = 0; i < num_orders; i++) {
    uint64_t idB = lob.generateID();
    uint64_t idA = lob.generateID();

    orders.push_back(OrdersGenerator(100, 'A', idA));
    orders.push_back(OrdersGenerator(100, 'B', idB));
    if (i % 100000 == 0) {
      lob.seeAskRank();
      std::cout << '\n';
      lob.seeBidRank();
    }
  }

  // Measurement Loop
  for (auto _ : state) {
    for (auto &ord : orders) {
      lob.addOrder(ord);
    }

    // Forces compiler to finish all memory writes before proceeding
    benchmark::ClobberMemory();

    state.PauseTiming();
    lob.reset(); // Crucial: Clear the state for the next iteration
    state.ResumeTiming();
  }

  state.SetItemsProcessed(state.iterations() * num_orders * 2);
}

BENCHMARK(LOB_Naive_Continuous)->Unit(benchmark::kMicrosecond);
BENCHMARK_MAIN();

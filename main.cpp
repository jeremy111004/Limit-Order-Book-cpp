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

  return {id, static_cast<uint32_t>(distribP(gen)),
          static_cast<uint32_t>(distribQ(gen)), type};
}

static void LOB_Naive_Continuous(benchmark::State &state) {
  LOB lob;
  std::vector<Order> orders;
  int num_orders = 1000000000;
  for (int i = 0; i < num_orders; i++) {
    uint64_t idB = lob.generateID();
    uint64_t idA = lob.generateID();

    auto orderBid = OrdersGenerator(100, 'B', idB);
    auto orderAsk = OrdersGenerator(100, 'A', idA);
    orders.push_back(orderAsk);
    orders.push_back(orderBid);
  }

  for (auto _ : state) {
    for (const auto &ord : orders) {
      if (ord.type == 'A') {
        lob.addAsk(ord);
      } else {
        lob.addBid(ord);
      }

      lob.matching();
    }
    benchmark::DoNotOptimize(lob);

    state.PauseTiming();
    lob.reset();
    state.ResumeTiming();
  }
  state.SetItemsProcessed(state.iterations() * num_orders);
}

BENCHMARK(LOB_Naive_Continuous)->Unit(benchmark::kMicrosecond);
BENCHMARK_MAIN();

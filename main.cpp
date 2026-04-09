#include "OrderBook.hpp"
#include <benchmark/benchmark.h>
#include <benchmark/benchmark_api.h>
#include <benchmark/registration.h>
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
               .next = nullptr,
               .prev = nullptr,
               .price = static_cast<uint32_t>(distribP(gen)),
               .quantity = static_cast<uint32_t>(distribQ(gen)),
               .type = type,
               .enabled = true};
}

static void LOB_Naive_Continuous(benchmark::State &state) {
  LOB LobForGeneration;
  const int numberOrders = 10000;
  std::vector<Order> SyntheticData;
  for (int c = 0; c < numberOrders; c++) {

    auto IDask = LobForGeneration.generateID();
    auto IDbid = LobForGeneration.generateID();
    auto ordAsk = OrdersGenerator(100, 'A', IDask);
    auto ordBid = OrdersGenerator(100, 'B', IDbid);
    SyntheticData.push_back(ordAsk);
    SyntheticData.push_back(ordBid);
  }
  for (auto _ : state) {
    state.PauseTiming();
    LOB lob;
    state.ResumeTiming();
    for (int i = 0; i < numberOrders; i++) {
      // otherwise the model would drain the quantity of orders in iteration 1
      // and leave the other iterations loop on 0 quantity
      Order ordcopy = SyntheticData[i];
      lob.addOrder(ordcopy);
    }

    benchmark::ClobberMemory();
  }
  state.SetItemsProcessed(state.iterations() * numberOrders);
}
BENCHMARK(LOB_Naive_Continuous);

#include "OrderBook.hpp"
#include <cstdint>
#include <iostream>
#include <random>

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

int main() {
  LOB lob;

  // Simulation: Add 300 Bids and 300 Asks
  for (int i = 0; i < 300; i++) {
    uint64_t idB = lob.generateID();
    uint64_t idA = lob.generateID();

    auto orderBid = OrdersGenerator(100, 'B', idB);
    auto orderAsk = OrdersGenerator(100, 'A', idA);

    lob.addBid(orderBid);
    lob.addAsk(orderAsk);
  }

  // Since this is a batch/naive model for now, we call matching manually
  std::cout << "Starting matching process..." << std::endl;
  lob.matching();

  std::cout << "--- Current Bid Book ---";
  lob.seeBidRank();

  std::cout << "\n\n--- Current Ask Book ---";
  lob.seeAskRank();

  return 0;
}

#pragma once
#include "Order.hpp"
#include <atomic>
#include <boost/circular_buffer.hpp>
#include <cstdint>
#include <functional>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>
using PriceLevel = boost::circular_buffer<Order *>;
class LOB {
public:
  // Core API
  void addAsk(const Order &ord);
  void addBid(const Order &ord);
  void matching();
  void cancelOrder(uint64_t id);
  uint64_t editOrder(uint64_t id, uint32_t newPrice, uint32_t newQuantity);

  // Utilities
  uint64_t generateID();
  void seeBidRank() const;
  void seeAskRank() const;

private:
  // price level with orders queued
  std::map<uint32_t, PriceLevel> priceMapAsk;
  std::map<uint32_t, PriceLevel, std::greater<uint32_t>> priceMapBid;

  // Maps ID to the specific position in the price level list to retrieve Orders
  // faster.
  std::unordered_map<uint64_t, PriceLevel::iterator> orderMap;

  std::vector<matchResult> matchedList;
};

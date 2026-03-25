#pragma once
#include "Order.hpp"
#include <atomic>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>

class LOB {
public:
  // Core API
  void addAsk(const Order &ord);
  void addBid(const Order &ord);
  void matching();
  void cancelOrder(uint64_t id);
  uint64_t editOrder(uint64_t id, uint32_t newPrice, uint32_t newQuantity);
  void reset();

  // Utilities
  uint64_t generateID();
  void seeBidRank() const;
  void seeAskRank() const;

private:
  // price level with orders queued
  std::map<uint32_t, std::list<Order>> priceMapAsk;
  std::map<uint32_t, std::list<Order>, std::greater<uint32_t>> priceMapBid;

  // Maps ID to the specific position in the price level list to retrieve Orders
  // faster.
  std::unordered_map<uint64_t, std::list<Order>::iterator> orderMap;

  std::vector<matchResult> matchedList;
};

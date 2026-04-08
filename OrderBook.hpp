#pragma once
#include "Arena.hpp"
#include "Order.hpp"
#include <boost/circular_buffer.hpp>
#include <boost/circular_buffer/base.hpp>
#include <list>
#include <map>
#include <sys/types.h>
#include <unordered_map>
#include <vector>
struct PriceList {
  Order *head = nullptr; // contains the pointer to the highest priority order
  Order *tail = nullptr; // contains the pointer to the latest Order
  u_int32_t numberOfOrders = 0;
  u_int64_t totalVolume = 0;
};
class LOB {
public:
  // Core API

  template <typename OppositeMap, typename Tcompare>
  void matching(Order &ord, OppositeMap &oppositeMap, Tcompare tcompare);
  void addOrder(Order &ord);
  void cancelOrder(uint64_t id);
  uint64_t editOrder(uint64_t id, uint32_t newPrice, uint32_t newQuantity);
  void reset();

  // Utilities
  uint64_t generateID();
  void seeBidRank() const;
  void seeAskRank() const;

private:
  // price level with orders queued
  std::map<uint32_t, PriceList> priceMapAsk;
  std::map<uint32_t, PriceList, std::greater<uint32_t>> priceMapBid;

  // Maps ID to the specific position in the price level list to retrieve Orders
  // faster.
  struct orderEntry {
    Order *ordptr;
    PriceList *levelptr;
  };
  std::unordered_map<uint64_t, orderEntry> orderMap;

  boost::circular_buffer<matchResult> matchedList;
  static constexpr auto daily_size = 1024 * 1024 * 1024;
  Arena arena{daily_size};
};

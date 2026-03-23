#include "OrderBook.hpp"
#include <atomic>
#include <cstdint>

#include <iostream>
#include <list>
#include <map>

#include <unordered_map>
#include <utility>
#include <vector>

uint64_t LOB::generateID() {
  static std::atomic<uint64_t> nextID{1};
  return nextID.fetch_add(1, std::memory_order_relaxed);
}

void LOB::addAsk(const Order &ord) {
  std::list<Order> &listAtPriceLevel = (priceMapAsk[ord.price]);
  listAtPriceLevel.push_back(ord);
  orderMap[ord.id] = std::prev(listAtPriceLevel.end());
}

void LOB::addBid(const Order &ord) {
  std::list<Order> &listAtPriceLevel = (priceMapBid[ord.price]);
  listAtPriceLevel.push_back(ord);
  orderMap[ord.id] = std::prev(listAtPriceLevel.end());
}

void LOB::matching() {
  // As long as there're price levels in those lists and the bids are
  // greater or equal than the asks, we loop.
  while (!priceMapBid.empty() && !priceMapAsk.empty() &&
         (priceMapBid.begin()->first) >= priceMapAsk.begin()->first) {
    std::list<Order> &list_of_asks = priceMapAsk.begin()->second;
    std::list<Order> &list_of_bids = priceMapBid.begin()->second;
    bool isAsksEmptyatPLVL{0};
    bool isBidsEmptyatPLVL{0};
    // as long as in each price level we have orders, we match them.
    while (!list_of_asks.empty() && !list_of_bids.empty()) {
      // now we match them until one quantity is == 0;

      auto &firstAsk = list_of_asks.front();
      auto &firstBid = list_of_bids.front();
      bool more_asks = firstAsk.quantity > firstBid.quantity;
      uint32_t quantityMatched = 0;
      if (more_asks) {
        quantityMatched = firstBid.quantity;
        firstBid.quantity = 0;
        firstAsk.quantity -= quantityMatched;
      } else {
        quantityMatched = firstAsk.quantity;
        firstAsk.quantity = 0;
        firstBid.quantity -= quantityMatched;
      }
      // we check who's empty;
      bool isTheAskEmpty = firstAsk.quantity == 0;
      bool isTheBidEmpty = firstBid.quantity == 0;

      matchResult matched = {firstAsk.id, firstBid.id, quantityMatched,
                             firstAsk.price};
      if (isTheAskEmpty) {
        orderMap.erase(firstAsk.id);
        list_of_asks.pop_front();
      }
      if (isTheBidEmpty) {
        orderMap.erase(firstBid.id);
        list_of_bids.pop_front();
      }

      matchedList.push_back(matched);
    }
    isAsksEmptyatPLVL = list_of_asks.empty();
    isBidsEmptyatPLVL = list_of_bids.empty();
    if (isAsksEmptyatPLVL) {
      priceMapAsk.erase(priceMapAsk.begin());
    }
    if (isBidsEmptyatPLVL) {
      priceMapBid.erase(priceMapBid.begin());
    }
  }
}

void LOB::cancelOrder(uint64_t id) {
  // safety check, exist is an iterator to a pair
  // (ID,std::list<Order>::iterator)
  auto exist = orderMap.find(id);
  if (exist == orderMap.end())
    return;
  // this is the orderIterator, that contains its address in the Ask/Bid price
  // map
  auto ordIterator = exist->second;

  auto isAnAsk = ordIterator->type == 'A';
  auto priceLevel = ordIterator->price;
  if (isAnAsk) {
    // itpriceLvl is an iterator to a pair (price Level, std::list<Order>)
    auto itpriceLvl = priceMapAsk.find(priceLevel);
    auto &listAtPriceLevel = itpriceLvl->second;
    // ordIterator contains the * of the Order so we can use it to delete from
    // the list directly
    listAtPriceLevel.erase(ordIterator);
    // itpriceLvl contains the * of the price Level and list.
    if (listAtPriceLevel.empty())
      priceMapAsk.erase(itpriceLvl);

  } else {
    auto itpriceLvl = priceMapBid.find(priceLevel);
    auto &listAtPriceLevel = itpriceLvl->second;
    listAtPriceLevel.erase(ordIterator);
    if (listAtPriceLevel.empty())
      priceMapBid.erase(itpriceLvl);
  }
  // exist contains the exact position of the ID + iterator for this order
  orderMap.erase(exist);
}

uint64_t LOB::editOrder(uint64_t id, uint32_t newPrice, uint32_t newQuantity) {
  // we check if it's a valid ID
  auto it = orderMap.find(id);
  if (it == orderMap.end())
    return 0;
  auto &existingOrder = *(it->second);

  char type = existingOrder.type;
  uint32_t oldPrice = existingOrder.price;
  uint32_t oldQuantity = existingOrder.quantity;

  // if price changed or quantity increased the priority will be gone.
  bool priceChanged = (oldPrice != newPrice);
  bool quantityIncreased = (newQuantity > oldQuantity);

  if (priceChanged || quantityIncreased) {
    cancelOrder(id);
    uint64_t newId = generateID();
    Order newOrder = {newId, newPrice, newQuantity, type};
    if (type == 'A') {
      addAsk(newOrder);
    } else {
      addBid(newOrder);
    }
    return newId;
  } else {
    existingOrder.quantity = newQuantity;
    return id;
  }
}
// those two functions simply print the Orders priorities
// within the different price levels
void LOB::seeBidRank() const {
  for (auto &g : priceMapBid) {
    std::cout << '\n'
              << "price level is " << g.first
              << " and the orders priorities are: ";
    for (auto &i : g.second) {
      std::cout << i.id << " ";
    }
  }
}

void LOB::seeAskRank() const {
  for (auto &g : priceMapAsk) {
    std::cout << '\n'
              << "price level is " << g.first
              << " and the orders priorities are: ";
    for (auto &i : g.second) {
      std::cout << i.id << " ";
    }
  }
}

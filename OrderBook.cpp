#include "OrderBook.hpp"
#include "Order.hpp"
#include <atomic>
#include <cstdint>
#include <iostream>
#include <map>
#include <unordered_map>
#include <utility>

uint64_t LOB::generateID() {
  static std::atomic<uint64_t> nextID{1};
  return nextID.fetch_add(1, std::memory_order_relaxed);
}

void LOB::reset() {
  priceMapAsk.clear();
  priceMapBid.clear();
  orderMap.clear();
  matchedList.clear();
}

template <typename OppositeMap, typename Tcompare>
void LOB::matching(Order &ord, OppositeMap &oppositeMap, Tcompare tcompare) {
  while (ord.quantity > 0 && !oppositeMap.empty()) {
    auto it =
        oppositeMap.begin(); // this is a iterator (price, price level list)
    uint32_t current_best_price = it->first;

    if (tcompare(
            ord.price,
            current_best_price)) { // tcompare is here bc we don't use the same
      //  comparison for ask -> bid or bid -> ask.
      break; // we break if the best bid/ask price isn't matching our ask/bid
             // price.
    }
    auto &lstPriceLevel = it->second;
    // we drain the orders at the found price level :
    while (ord.quantity > 0 && lstPriceLevel.head != nullptr) {
      auto &matchedOrder = *lstPriceLevel.head;
      uint32_t qmatched = std::min(ord.quantity, matchedOrder.quantity);
      ord.quantity -= qmatched;
      matchedOrder.quantity -= qmatched;

      // Metadata draining logic
      lstPriceLevel.totalVolume -=
          static_cast<uint64_t>(qmatched) * current_best_price;

      matchedList.push_back(
          {ord.id, matchedOrder.id, current_best_price, qmatched});
      // removal and update logic
      if (matchedOrder.quantity == 0) {
        lstPriceLevel.numberOfOrders -= 1; // Metadata draining logic

        orderMap.erase(matchedOrder.id);
        auto &newHead = matchedOrder.next;
        if (newHead != nullptr) {
          newHead->prev = nullptr;
        }
        if (matchedOrder.next == nullptr) {
          (lstPriceLevel).tail = nullptr;
        }
        lstPriceLevel.head = newHead;
      }
    }

    if (lstPriceLevel.head == nullptr && lstPriceLevel.tail == nullptr) {
      oppositeMap.erase(it);
    }
  }
}

void LOB::addOrder(Order &ord) {
  if (ord.type == 'B') {
    matching(ord, priceMapAsk, std::less<uint32_t>());
  } else {
    matching(ord, priceMapBid, std::greater<uint32_t>());
  };

  // if the order hasn't been fully matched (or not matched at all) we have to
  // actually save it:
  if (ord.quantity > 0) {
    // this line create an order directly in the pre allocated memory Arena
    Order *order_ptr = new (arena.allocate(sizeof(Order))) Order(ord);

    auto &priceLevel =
        (ord.type == 'B') ? priceMapBid[ord.price] : priceMapAsk[ord.price];

    if (priceLevel.head == nullptr) { // if the price level is empty we use
                                      // the order as the tail and the head
      priceLevel.head = order_ptr;
      priceLevel.tail = order_ptr;
    } else {
      // we link the current order with the last tail;
      order_ptr->prev = priceLevel.tail;
      priceLevel.tail->next = order_ptr;
      priceLevel.tail =
          order_ptr; // and now the current order becomes the tail,;
    }

    priceLevel.numberOfOrders += 1;
    priceLevel.totalVolume += static_cast<uint64_t>(ord.quantity) * ord.price;

    // just adding the order coordinates to the map;
    orderMap[ord.id] = {order_ptr, &priceLevel};
  }
}

void LOB::cancelOrder(uint64_t id) {
  auto exist = orderMap.find(id);
  if (exist == orderMap.end())
    return;
  auto ordEntry = exist->second;
  auto ordptr = ordEntry.ordptr;
  auto listAtPriceLevel = ordEntry.levelptr;

  // Neighbor linking
  if (ordptr->prev) {
    ordptr->prev->next = ordptr->next;
  } else { // if there's no prev it was a head
    listAtPriceLevel->head =
        ordptr->next; // so the new head is either nullptr or ordptr->next;
  }
  if (ordptr->next) {
    ordptr->next->prev = ordptr->prev;
  } else { // if there's no next then it was a tail
    listAtPriceLevel->tail = ordptr->prev;
  }

  // we decrement the metadata of the price level
  listAtPriceLevel->numberOfOrders--;
  // Use 64 bit promotion to prevent overflow before subtraction
  listAtPriceLevel->totalVolume -=
      static_cast<uint64_t>(ordptr->price) * ordptr->quantity;

  ordptr->next = nullptr;
  ordptr->prev = nullptr;

  orderMap.erase(id);
}

void LOB::seeBidRank() const {
  for (auto &g : priceMapBid) {
    std::cout << '\n'
              << "Bids price level is " << g.first
              << " and the orders priorities are: ";
    Order *dummyptr = g.second.head;
    for (int i = 0; i < g.second.numberOfOrders; i++) {
      std::cout << "  " << dummyptr->id;

      if (!dummyptr->next)
        break;
      dummyptr = dummyptr->next;
    }
  }
}

void LOB::seeAskRank() const {
  for (auto &g : priceMapAsk) {
    std::cout << '\n'
              << "Asks price level is " << g.first
              << " and the orders priorities are: ";
    Order *dummyptr = g.second.head;
    for (int i = 0; i < g.second.numberOfOrders; i++) {
      std::cout << "  " << dummyptr->id;
      if (!dummyptr->next)
        break;
      dummyptr = dummyptr->next;
    }
  }
}

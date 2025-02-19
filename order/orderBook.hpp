#pragma once
#include "order.hpp"
#include "../userBalances/userBalances.hpp"
#include <iostream>
#include <ctime>
#include <set>
#include <vector>
#include <mutex>
#include <unordered_map>

// Construct the order book for 1 single sticker. 
// Mapping order to the correct ticker's book is the responsibility of the orderMediator (TODO)

template<int sign>
struct orderComparator {
	bool operator() (const order& a, const order& b) const {
		if (sign * (a.price - b.price) < 0) {
			return true;
		}

		double timeDiff = std::difftime(a.submitTime, b.submitTime);
		if (a.price == b.price && timeDiff < 0) {
			return true;
		}

		return false;
	}
};

typedef std::set<order, orderComparator<1>> sell_set;
typedef std::set<order, orderComparator<-1>> buy_set;
typedef std::unordered_map<std::string, sell_set::iterator> sell_map;
typedef std::unordered_map<std::string, buy_set::iterator> buy_map;
class orderBook {
private:
	std::mutex buySideMutex;
	std::mutex sellSideMutex;
	sell_set sellSide;
	buy_set buySide;

	std::mutex sellSideIterMapMutex;
	std::mutex buySideIterMapMutex;
	sell_map sellSideIterMap;
	buy_map buySideIterMap;

	userBalances& userStorage;
public:
	orderBook(userBalances& userStorage):userStorage(userStorage){};
	// Return the list of matched orders 
	std::vector<order> addOrder(order newOrder);

	template<class T, class TMap, class U, class UMap>
	std::vector<order> addOrderHelper(order newOrder,
		T& oppositeSide, std::mutex& oppositeSideMutex,
		TMap& oppositeMap, std::mutex& oppositeMapMutex,
		U& holdSide, std::mutex& holdSideMutex,
		UMap& holdMap, std::mutex& holdMapMutex) {

		std::vector<order> result;
		// Lock the resources while updating it.
		{
			std::lock_guard<std::mutex> lock(oppositeSideMutex);
			std::lock_guard<std::mutex> lockMap(oppositeMapMutex);

			// Iterate through the oppositeSide
			auto startIter = oppositeSide.begin();
			auto endIter = oppositeSide.begin();

			for (auto it = oppositeSide.begin(); it != oppositeSide.end(); it++) {
				if (newOrder.amount == 0) {
					endIter = it;
					break;
				}

				if ((!newOrder.sell && it->price > newOrder.price) || (newOrder.sell && it->price < newOrder.price)) {
					endIter = it;
					break;
				}

				order removedOrder(*it);
				if (newOrder.amount < it->amount) {
					removedOrder.amount = newOrder.amount;
					it->amount -= newOrder.amount;
					endIter = it;

					newOrder.amount = 0;
					result.push_back(removedOrder);

					int priceChange = removedOrder.amount * removedOrder.price * (newOrder.sell ? 1 : -1);
					userStorage.setBalance(newOrder.userId, userStorage.getBalance(newOrder.userId) 
						+ priceChange);
					userStorage.setBalance(it->userId, userStorage.getBalance(it->userId)
						- priceChange);

					break;
				}
				else {
					newOrder.amount -= it->amount;
					oppositeMap.erase(it->orderId);
					endIter = std::next(it, 1);
					result.push_back(removedOrder);

					int priceChange = removedOrder.amount * removedOrder.price * (newOrder.sell ? 1 : -1);
					userStorage.setBalance(newOrder.userId, userStorage.getBalance(newOrder.userId)
						+ priceChange);
					userStorage.setBalance(it->userId, userStorage.getBalance(it->userId)
						- priceChange);


				}
			}

			if (endIter != oppositeSide.begin()) {
				oppositeSide.erase(startIter, endIter);
			}
		}

		if (newOrder.amount > 0) {
			std::lock_guard<std::mutex> lock(holdSideMutex);
			std::lock_guard<std::mutex> lockMap(holdMapMutex);

			auto check = holdSide.insert(newOrder);
			if (check.second) {
				holdMap[newOrder.orderId] = check.first;
			}
			else {
				throw std::runtime_error("Cannot insert order " + newOrder.orderId);
			}
		}

		std::cout << holdSide.size() << std::endl;

		return result;
	};

	bool removeOrder(std::string& orderId);
	
	void displayOrder() const;

	std::string stringify() const;

	std::string stringifySummary() const;
};
#pragma once
#include "order.hpp"
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
public:
	// Return the list of matched orders 
	std::vector<order> addOrder(order newOrder);

	template<class T, class TMap, class U, class UMap>
	std::vector<order> addOrderHelper(order newOrder,
		T& oppositeSide, std::mutex& oppositeSideMutex,
		TMap& oppositeMap, std::mutex& oppositeMapMutex,
		U& holdSide, std::mutex& holdSideMutex,
		UMap& holdMap, std::mutex& holdMapMutex);

	bool removeOrder(std::string& orderId);
	
	void displayOrder();
};
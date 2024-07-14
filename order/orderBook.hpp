#pragma once
#include "order.hpp"
#include <iostream>
#include <ctime>
#include <set>
#include <vector>
#include <mutex>

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
class orderBook {
public:
	// TODO: Move these to private after done
	std::mutex bookSidesMutex;
	sell_set sellSide;
	buy_set buySide;



	// Return the list of matched orders 
	// TODO: Check this
	std::vector<order> addOrder(order newOrder);

	template<class T, class U>
	std::vector<order> addOrderHelper(order newOrder, T& oppositeSide, U& holdSide);
	
	void displayOrder();
};
#pragma once
#include "order.hpp"
#include <ctime>
#include <set>
#include <vector>

// Construct the order book for 1 single sticker. 
// Mapping order to the correct ticker's book is the responsibility of the orderMediator (TODO)

template<int sign>
struct orderComparator {
	bool operator() (order a, order b) const {
		if (sign * (a.price - b.price) < 0) {
			return true;
		}

		double timeDiff = std::difftime(a.submitTime, b.submitTime);
		if (timeDiff < 0) {
			return true;
		}

		return false;
	}
};

class orderBook {
public:
	// TODO: Move these to private after done
	std::set<order, orderComparator<1>> sellSide;
	std::set<order, orderComparator<-1>> buySide;

	// Return the list of matched orders 
	// TODO: Check this
	std::vector<order> addOrder(order newOrder);
};
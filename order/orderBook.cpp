#include "orderBook.hpp"

std::vector<order> orderBook::addOrder(order newOrder) {
	vector<order> result;
	std::set<order>& side = sellSide;
	std::set<order>& holdSide = buySide;

	if (!newOrder.sell) {
		side = buySide;
		holdSide = sellSide;
	}

	// Iterate through the sell side
	auto startIter = side.begin();
	auto endIter = side.begin();

	for (auto it = side.begin(); it != side.end(); it++) {
		if (newOrder.amount == 0) {
			endIter = it;
			break;
		}

		if (it->price > newOrder.price) {
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
		}
		else {
			newOrder.amount -= it->amount;
			result.push_back(removedOrder);
		}
	}

	if (endIter != side.begin()) {
		side.erase(startIter, endIter);
	}

	if (newOrder->amount > 0) {
		holdSide.push_back(newOrder);
	}

	return result;
}
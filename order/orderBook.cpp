#include "orderBook.hpp"

std::vector<order> orderBook::addOrder(order newOrder) {
	if (newOrder.sell) {
		return this->addOrderHelper<buy_set, sell_set>(newOrder, this->buySide, this->sellSide);
	}
	else {
		return this->addOrderHelper<sell_set, buy_set>(newOrder, this->sellSide, this->buySide);
	}
}

template<class T, class U>
std::vector<order> orderBook::addOrderHelper(order newOrder, T& oppositeSide, U& holdSide) {
	std::vector<order> result;
	// Lock the resources while updating it. TODO: Split into 2 locks for each side
	std::lock_guard<std::mutex> lock(this->bookSidesMutex);

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
			break;
		}
		else {
			newOrder.amount -= it->amount;
			result.push_back(removedOrder);
		}
	}

	if (endIter != oppositeSide.begin()) {
		oppositeSide.erase(startIter, endIter);
	}

	if (newOrder.amount > 0) {
		holdSide.insert(newOrder);
	}

	std::cout << holdSide.size() << std::endl;

	return result;
}

void orderBook::displayOrder() {
	std::cout << "Buy side" << this->buySide.size() << std::endl;
	for (order order : this->buySide) {
		order.printOrder();
	}

	std::cout << "Sell side" << this->sellSide.size() << std::endl;
	for (order order : this->sellSide) {
		order.printOrder();
	}
}
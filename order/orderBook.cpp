#include "orderBook.hpp"
#include <stdexcept>

std::vector<order> orderBook::addOrder(order newOrder) {
	if (newOrder.sell) {
		return this->addOrderHelper<buy_set, buy_map, sell_set, sell_map>(newOrder, 
			this->buySide, this->buySideMutex,
			this->buySideIterMap, this->buySideIterMapMutex,
			this->sellSide, this->sellSideMutex,
			this->sellSideIterMap, this->sellSideIterMapMutex
		);
	}
	else {
		return this->addOrderHelper<sell_set, sell_map, buy_set, buy_map>(newOrder, 
			this->sellSide, this->sellSideMutex, 
			this->sellSideIterMap, this->sellSideIterMapMutex,
			this->buySide, this->buySideMutex,
			this->buySideIterMap, this->buySideIterMapMutex
		);
	}
}

template<class T, class TMap, class U, class UMap>
std::vector<order> orderBook::addOrderHelper(order newOrder, 
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
				break;
			}
			else {
				newOrder.amount -= it->amount;
				oppositeMap.erase(it->orderId);
				result.push_back(removedOrder);
			}
		}

		if (endIter != oppositeSide.begin()) {
			oppositeSide.erase(startIter, endIter);
		}
	}

	if (newOrder.amount > 0) {
		std::lock_guard<std::mutex> lock(holdSideMutex);
		std::lock_guard<std::mutex> lockMap(holdMapMutex);

		auto result = holdSide.insert(newOrder);
		if (result.second) {
			holdMap[newOrder.orderId] = result.first;
		}
		else {
			throw std::runtime_error("Cannot insert order " + newOrder.orderId);
		}
	}

	std::cout << holdSide.size() << std::endl;

	return result;
}

bool orderBook::removeOrder(std::string& orderId) {
	if (this->sellSideIterMap.count(orderId) == 1) {
		sell_set::iterator it;
		{
			std::lock_guard<std::mutex> lock(this->sellSideIterMapMutex);
			it = sellSideIterMap[orderId];
			this->sellSideIterMap.erase(orderId);
		}
		{
			std::lock_guard<std::mutex> lock(this->sellSideMutex);
			this->sellSide.erase(it);
		}
		return true;
	}
	else if (this->buySideIterMap.count(orderId) == 1){
		buy_set::iterator it;
		{
			std::lock_guard<std::mutex> lock(this->buySideIterMapMutex);
			it = buySideIterMap[orderId];
			this->buySideIterMap.erase(orderId);
		}
		{
			std::lock_guard<std::mutex> lock(this->buySideMutex);
			this->buySide.erase(it);
		}
		return true;
	}
	else {
		return false;
	}
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
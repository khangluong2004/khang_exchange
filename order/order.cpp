#include "order.hpp"
#include <string>

order::order(order& const oldOrder) {
	this->price = oldOrder.price;
	this->amount = oldOrder.amount;
	this->sell = oldOrder.sell;
	this->submitTime = oldOrder.submitTime;
	this->ticker = oldOrder.ticker;
	this->userId = oldOrder.userId;
}
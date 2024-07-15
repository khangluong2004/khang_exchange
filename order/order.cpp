#include "order.hpp"

order::order(const order& oldOrder) {
	this->price = oldOrder.price;
	this->amount = oldOrder.amount;
	this->sell = oldOrder.sell;
	this->submitTime = oldOrder.submitTime;
	this->orderId = oldOrder.orderId;
	this->ticker = oldOrder.ticker;
	this->userId = oldOrder.userId;
}

order::order(const Json::Value& messageObj) {
	this->price = messageObj["price"].asDouble();
	this->amount = messageObj["amount"].asInt();
	this->sell = messageObj["sell"].asBool();
	this->submitTime = time(NULL);
	this->ticker = messageObj["ticker"].asString();
	this->userId = messageObj["userId"].asString();
	this->orderId = messageObj["orderId"].asString();
}

void order::printOrder() const {
	std::cout << this->orderId << ": From " << userId << " for " << ticker << " sell: " << this->sell << std::endl;
	std::cout << "Order {" << "price: " << this->price << ", amount: " << this->amount << ", time: " << this->submitTime << std::endl;
}
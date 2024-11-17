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
	this->price = messageObj["price"].asInt();
	this->amount = messageObj["amount"].asInt();
	this->sell = messageObj["sell"].asBool();
	this->submitTime = time(NULL);
	this->ticker = messageObj["ticker"].asString();
	this->userId = messageObj["userId"].asString();
	this->orderId = messageObj["orderId"].asString();
}

void order::printOrder() const {
	std::cout << this->stringify() << std::endl;
}

std::string order::stringify() const {
	std::string jsonStringify = "{ \"from\": \"" + this->userId + "\"" +
		", \"orderId\": \"" + this->orderId + "\"" +
		", \"ticker\": \"" + this->ticker + "\"" +
		", \"price\": " + std::to_string(this->price) + 
		", \"amount\": " + std::to_string(this->amount) + 
		", \"time\": " + std::to_string(this->submitTime) +
		", \"isSell\": " + (this->sell ? "true": "false") + "}";
	return (jsonStringify);
}
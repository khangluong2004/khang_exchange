#pragma once

// Order structure: {userId: string, tickers: string, price: double, amount: int, sell: boolean, submitTime: time_t}
#include <string>
#include <ctime>
#include <iostream>
#include <json/json.h>

class order {
public:
	std::string userId;
	std::string orderId; // Computed by appending a count to userId: eg: <userId>-<order-count>
	std::string ticker;
	mutable int amount;
	int price; // in cents
	bool sell;
	time_t submitTime;

	order(const order& oldOrder);
	order(const Json::Value& messageObj);
	void printOrder() const;
	std::string stringify() const;
};
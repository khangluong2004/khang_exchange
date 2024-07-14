#pragma once

// Order structure: {userId: string, tickers: string, price: double, amount: int, sell: boolean, submitTime: time_t}
#include <string>
#include <ctime>

class order {
public:
	std::string userId;
	std::string ticker;
	int amount;
	double price;
	bool sell;
	time_t submitTime;

	order(order& const oldOrder);
};
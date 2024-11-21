#include "userBalances.hpp"

void userBalances::setBalance(const std::string& userId, int value) {
	this->localStorage[userId] = value;
}

int userBalances::getBalance(const std::string& userId) {
	return this->localStorage[userId];
}
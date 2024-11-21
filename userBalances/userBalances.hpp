#pragma once
#include <string>
#include <unordered_map>

/// <summary>
/// Use to store userBalances in memory for now. Can refactor to use more fancy db if needed.
/// Store balances in cents.
/// </summary>
class userBalances {
private:
	std::unordered_map<std::string, int> localStorage;
public:
	void setBalance(const std::string& userId, int value);
	int getBalance(const std::string& userId);
};
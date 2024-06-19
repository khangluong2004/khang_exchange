#pragma once

#include "logger.hpp"
#include <fstream>
#include <string>

// Include expensive IO for each update
// Implemented for simplicity for now, add buffer later for better efficiency
class textLogger : public logger {
private:
	std::string filename;
public:
	textLogger(std::string filename);
	void loggingInfo(const Json::Value& messageObj);
};
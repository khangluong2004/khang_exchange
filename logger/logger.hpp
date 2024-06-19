#pragma once
#include <json/json.h>

class logger {
public:
	virtual void loggingInfo(const Json::Value& messageObj) = 0;
};
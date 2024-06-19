#include "textLogger.hpp"

textLogger::textLogger(std::string filename){
	this->filename = filename;
}

void textLogger::loggingInfo(const Json::Value& messageObj){
	Json::StreamWriterBuilder wbuilder;
	wbuilder["commentStyle"] = "None";
	wbuilder["indentation"] = "";
	std::string value = Json::writeString(wbuilder, messageObj);

	std::ofstream of;
	of.open(this->filename, std::ios::out | std::ios::app);
	of << value << std::endl;
	of.close();
}
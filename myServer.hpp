#pragma once
// Use for standalone version of Asio 
// Remove if using Boost Asio (TODO)
#define ASIO_STANDALONE

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <json/json.h>

#include <functional>
#include <iostream>
#include <algorithm>

#include <string>
#include <vector>
#include <map>

/// <summary>
/// Websocket:
/// 1. Endpoint: Creates and launches connection with its settings
/// 2. Connection: Facilitates transfer of data. Once created, is not synced
/// with endpoints
/// 
/// Endpoints and connection exists independently
/// </summary>

// Typedef for server (endpoint), connection and message
typedef websocketpp::server<websocketpp::config::asio> asioServer;
typedef websocketpp::connection_hdl ClientConnection;
typedef asioServer::message_ptr message_ptr;

class myServer {
private:
	asioServer m_endpoint;
	std::vector<ClientConnection> allConnections;

	// Storing handlers for event-driven pattern
	// Register handlers run on the io network thread,
	// but running them is on the io main thread
	std::vector<std::function<void(ClientConnection)>> openHandlers;
	std::vector<std::function<void(ClientConnection)>> closeHandlers;
	std::map<std::string, std::vector<std::function<void(ClientConnection, const Json::Value&)>>> messageHandlers;
protected:
	void onOpen(ClientConnection connection);
	void onClose(ClientConnection connection);
	void onMessage(ClientConnection connection, message_ptr msg);

	Json::Value parseJson(const std::string& json);
	std::string stringify(const Json::Value& val);
public:
	myServer();
	void run(int port = 8080);

	// Utility
	void sendJsonMessage(ClientConnection connection, const Json::Value& body);

	// Getter
	int getNumConnections();

	// Methods to register handler
	template<typename CallbackT>
	void addOpenHandler(CallbackT callback) {
		this->openHandlers.push_back(callback);
	};

	template<typename CallbackT>
	void addCloseHandler(CallbackT callback) {
		this->closeHandlers.push_back(callback);
	};

	template<typename CallbackT>
	void addMessageHandler(const std::string& messageType, CallbackT callback) {
		this->messageHandlers[messageType].push_back(callback);
	}
};
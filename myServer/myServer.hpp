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
#include <mutex>

#include <string>
#include <vector>
#include <map>

#include "../order/order.hpp"
#include "../order/orderBook.hpp"

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
	// Need lock to ensure thread-safe
	std::mutex connectionListMutex;
	std::vector<ClientConnection> allConnections;

	// Storing handlers for event-driven pattern
	// Register handlers cannot have race conditions, since the async listening only starts after that,
	// Running handlers is on the thread pool
	std::vector<std::function<void(ClientConnection)>> openHandlers;
	std::vector<std::function<void(ClientConnection)>> closeHandlers;
	std::map<std::string, std::vector<std::function<void(ClientConnection, const Json::Value&)>>> messageHandlers;

	// Set up the order book 
	orderBook book;

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
	void broadcast(const Json::Value& body);

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

	// Add order
	void addOrder(const order& newOrder);

	void displayOrder();
};
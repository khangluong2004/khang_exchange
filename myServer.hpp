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
protected:
	void onOpen(ClientConnection connection);

	void onClose(ClientConnection connection);

	void onMessage(ClientConnection connection, message_ptr msg);

	Json::Value parseJson(const std::string& json);
	std::string stringify(const Json::Value& val);
	void sendJsonMessage(ClientConnection connection, const Json::Value& body);
public:
	myServer();

	void run(int port = 8080);
};
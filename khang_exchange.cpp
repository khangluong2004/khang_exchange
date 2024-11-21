// khang_exchange.cpp : Defines the entry point for the application.
//

#include "khang_exchange.hpp"
#include "myServer/myServer.hpp"
#include "logger/logger.hpp"
#include "logger/textLogger.hpp"
#include "order/order.hpp"

int main()
{
	// Initialize key variables
	// Create a thread pool
	asio::thread_pool pool;

	// Logger
	textLogger textLogger(std::string("requests.txt"));
	logger& myLogger = textLogger;

	// WebSocketServer
	myServer server;
	// Initialize handlers to server
	// Those io operations will use the thread pool
	server.addOpenHandler(
		[&pool, &server](ClientConnection connection) {
			asio::post(pool, 
				[&server]() {
					std::cout << "Opened connection. Total connections: " << server.getNumConnections() << std::endl;
				}
			);
		}
	);

	server.addCloseHandler(
		[&pool, &server](ClientConnection connection) {
			asio::post(pool, 
				[&server]() {
					std::cout << "Closed connection. Total connections: " << server.getNumConnections() << std::endl;
				}
			);
		}
	);

	// Add echo message handlers
	server.addMessageHandler("echo",
		[&pool, &server](ClientConnection connection, const Json::Value& messageObj) {
			// Echo back the value for now
			asio::post(pool,
				[&server, connection, messageObj]() {
					server.sendJsonMessage(connection, messageObj);
				}
			);
		}
	);

	// Add order message handlers
	server.addMessageHandler("order",
		[&pool, &server](ClientConnection connection, const Json::Value& messageObj) {
			asio::post(pool,
				[&server, connection, messageObj]() {
					order newOrder(messageObj);
					server.addOrder(newOrder);
				}
			);
		}
	);

	// Add display order message handlers
	server.addMessageHandler("displayOrder",
		[&pool, &server](ClientConnection connection, const Json::Value& messageObj) {
			asio::post(pool,
				[&server, connection, messageObj]() {
					server.displayOrder();
				}
			);
		}
	);

	// Add retrieve order message handlers
	server.addMessageHandler("retrieveOrder",
		[&pool, &server](ClientConnection connection, const Json::Value& messageObj) {
			asio::post(pool,
			[&server, connection, messageObj]() {
				server.sendMessage(connection, server.stringifyOrder());
			}
			);
		}
	);

	// Add retrieve order summary message handlers
	server.addMessageHandler("retrieveSummary",
		[&pool, &server](ClientConnection connection, const Json::Value& messageObj) {
			asio::post(pool,
			[&server, connection, messageObj]() {
				server.sendMessage(connection, server.stringifySummary());
			}
			);
		}
	);

	// Add remove order message handlers
	server.addMessageHandler("removeOrder",
		[&pool, &server](ClientConnection connection, const Json::Value& messageObj) {
			asio::post(pool,
				[&server, connection, messageObj]() {
					std::string orderId = messageObj["orderId"].asString();
					server.removeOrder(orderId);
				}
			);
		}
	);

	// Add broadcast message handlers
	// Broadcast is run on the network thread
	server.addMessageHandler("broadcast",
		[&server](ClientConnection connection, const Json::Value& messageObj) {
			server.broadcast(messageObj);
		}
	);

	// Add logger message handlers
	// The logger process is run on the mainEventLoop thread
	server.addMessageHandler("logger",
		[&pool, &myLogger](ClientConnection connection, const Json::Value& messageObj) {
			asio::post(pool, 
				[&myLogger, messageObj]() {
					myLogger.loggingInfo(messageObj);
				}
			);
		}
	);

	// Add balance getter
	// Add remove order message handlers
	server.addMessageHandler("getBalance",
		[&pool, &server](ClientConnection connection, const Json::Value& messageObj) {
			asio::post(pool,
			[&server, connection, messageObj]() {
					std::string userId = messageObj["userId"].asString();
					server.sendMessage(connection, std::to_string(server.getBalance(userId)));
				}
			);
		}
	);

	// Start networking thread
	auto runServerFunc = [&server] {
		try {
			server.run();
		}
		catch (websocketpp::exception const& e) {
			std::cout << e.what() << std::endl;
		}
		catch (...) {
			std::cout << "other exception" << std::endl;
		}
	};
	std::thread networkThread(runServerFunc);

	// Wait until all tasks are completed
	// Use the networkThread to make sure all threads are kept alive until the network thread dies
	networkThread.join();

	// Other threads in the thread pools
	pool.join();

	return 0;
}

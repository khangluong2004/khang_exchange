// khang_exchange.cpp : Defines the entry point for the application.
//

#include "khang_exchange.hpp"
#include "myServer/myServer.hpp"
#include "logger/logger.hpp"
#include "logger/textLogger.hpp"

int main()
{
	// Initialize key variables
	// Main event loop
	asio::io_service mainEventLoop;

	// Logger
	textLogger textLogger(std::string("requests.txt"));
	logger& myLogger = textLogger;

	// WebSocketServer
	myServer server;
	// Initialize handlers to server
	// Those io operations will use the mainEventLoop to avoid blocking
	// the networking thread
	server.addOpenHandler(
		[&mainEventLoop, &server](ClientConnection connection) {
			mainEventLoop.post(
				[&server]() {
					std::cout << "Opened connection. Total connections: " << server.getNumConnections() << std::endl;
				}
			);
		}
	);

	server.addCloseHandler(
		[&mainEventLoop, &server](ClientConnection connection) {
			mainEventLoop.post(
				[&server]() {
					std::cout << "Closed connection. Total connections: " << server.getNumConnections() << std::endl;
				}
			);
		}
	);

	// Add echo message handlers
	server.addMessageHandler("echo",
		[&mainEventLoop, &server](ClientConnection connection, const Json::Value& messageObj) {
			// Echo back the value for now
			mainEventLoop.post(
				[&server, connection, messageObj]() {
					server.sendJsonMessage(connection, messageObj);
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
		[&mainEventLoop, &myLogger](ClientConnection connection, const Json::Value& messageObj) {
			mainEventLoop.post(
				[&myLogger, messageObj]() {
					myLogger.loggingInfo(messageObj);
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

	// Keep the main event loop runnning on the main thread
	asio::io_service::work work(mainEventLoop);
	mainEventLoop.run();

	return 0;
}

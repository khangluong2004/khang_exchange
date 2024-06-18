// khang_exchange.cpp : Defines the entry point for the application.
//

#include "khang_exchange.hpp"
#include "myServer.hpp"

int main()
{
	// Initialize key variables
	// Main event loop
	asio::io_service mainEventLoop;
	// WebSocketServer
	myServer server;


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

	// Start networking thread
	std::thread networkThread(runServerFunc);

	// Keep the main event loop runnning on the main thread
	asio::io_service::work work(mainEventLoop);
	mainEventLoop.run();

	return 0;
}

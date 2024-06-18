// khang_exchange.cpp : Defines the entry point for the application.
//

#include "khang_exchange.hpp"
#include "myServer.hpp"

int main()
{
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

	// Networking thread
	std::thread networkThread(runServerFunc);

	// Block the main thread until the network thread return
	networkThread.join();
	return 0;
}

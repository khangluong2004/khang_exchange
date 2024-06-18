#include "myServer.hpp";

void myServer::onOpen(ClientConnection connection) {
	std::cout << "Open connection" << std::endl;
}

void myServer::onClose(ClientConnection connection) {
	std::cout << "Close connection" << std::endl;
}

void myServer::onMessage(ClientConnection connection, message_ptr msg) {
	std::cout << "Received message" << std::endl;
	std::cout << "Opcode: " << msg->get_opcode() << std::endl;
	std::cout << "On Hdl channel: " << connection.lock().get() << std::endl;

	// Check for special command to stop the server
	/*if (msg->get_payload() == "stop-listening") {
		m_endpoint.stop_listening();
		return;
	}*/

	// Process payload in json

	Json::Value messageObj = this->parseJson(msg->get_payload());
	for (auto& key : messageObj.getMemberNames()) {
		std::cout << "\t" << key << ": " << messageObj[key].asString() << std::endl;
	}

	// Echo back the value for now
	try {
		m_endpoint.send(connection, msg->get_payload(), msg->get_opcode());
	}
	catch (websocketpp::exception const& e) {
		std::cout << "Echo failed because: "
			<< e.what() << std::endl;
	}
}

Json::Value myServer::parseJson(const std::string& json) {
	Json::Value root;
	Json::Reader reader;
	reader.parse(json, root);
	return root;
}

std::string myServer::stringify(const Json::Value& root) {
	Json::StreamWriterBuilder wbuilder;
	wbuilder["commentStyle"] = "None";
	wbuilder["indentation"] = "";
	return Json::writeString(wbuilder, root);
}

myServer::myServer() {
	// Set logging settings
	m_endpoint.set_error_channels(websocketpp::log::elevel::all);
	m_endpoint.set_access_channels(websocketpp::log::alevel::all ^
		websocketpp::log::alevel::frame_payload);

	// Bind handlers
	m_endpoint.set_open_handler(std::bind(&myServer::onOpen, this, std::placeholders::_1));
	m_endpoint.set_close_handler(std::bind(&myServer::onClose, this, std::placeholders::_1));
	m_endpoint.set_message_handler(std::bind(&myServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));

	// Initialize asio
	m_endpoint.init_asio();
}

void myServer::run() {
	// NEED CATCHING FOR EXCEPTION
	// Listen on port 8080
	m_endpoint.listen(8080);
	std::cout << "Server running on port 8080" << std::endl;

	// Queue connection accept operation
	m_endpoint.start_accept();

	// Start the Asio io_service run loop
	// Block until the endpoint is instructed to stop listening
	// Will listent + process new connection; and process data
	// + messages for existing connections;
	// Asynchronous asio
	m_endpoint.run();
}

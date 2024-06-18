#include "myServer.hpp";

void myServer::onOpen(ClientConnection connection) {
	std::cout << "Open connection" << std::endl;
	allConnections.push_back(connection);
}

void myServer::onClose(ClientConnection connection) {
	std::cout << "Close connection" << std::endl;

	// Promote the connection from weak pointer to shared pointer for comparison
	auto connectionVal = connection.lock();
	// Earse-remove idiom
	// "Remove" all expired and the correct connection
	// Unlike erase, remove if move the values to the end of the vector,
	// and change the end iterator
	auto newEnd = std::remove_if(allConnections.begin(), allConnections.end(), 
		[&connectionVal](const ClientConnection& element){
			// Remove expired weak pointer
			if (element.expired()) {
				return true;
			}

			// Remove the connection that is closed
			if (element.lock().get() == connectionVal.get()) {
				return true;
			}

			return false;
		}
	);

	// Erase all the element from new end to old end (removed elements)
	allConnections.erase(newEnd, allConnections.end());
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

void myServer::sendJsonMessage(ClientConnection connection, const Json::Value& body){
	try {
		m_endpoint.send(connection, myServer::stringify(body), websocketpp::frame::opcode::text);
	}
	catch (websocketpp::exception const& e) {
		std::cout << "Send message failed because: "
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

void myServer::run(int port) {
	// NEED CATCHING FOR EXCEPTION
	// Listen on port 8080
	m_endpoint.listen(port);
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

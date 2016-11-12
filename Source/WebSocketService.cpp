#include "WebSocketService.h"

std::vector<std::vector<std::string>> WebSocketService::sendTasks;

void WebSocketService::WSService () {
	Logger::log("WRLD", "WSTHRD", "Thread started.");
	using easywsclient::WebSocket;
	try {
		// TODO: Work on THAT
		//WebSocket::pointer ws = WebSocket::from_url("ws://coolgametube.net:2042/9435f280dd285d8cdc5fa8f418b9ced5cc6dfaf9");
		/*if (ws->getReadyState()!=WebSocket::CLOSED) {
			ws->send("goodbye");
			ws->send("hello");
		}
		
		while (ws->getReadyState() != WebSocket::CLOSED) {
			ws->poll();
			//ws->dispatch(handle_message);
		}*/
	}catch(...){}
}
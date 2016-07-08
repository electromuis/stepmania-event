#include "global.h"

#include "WebServer.h"
#include "RageLog.h"

WebServer* WEBSERVER = NULL;

namespace http = boost::network::http;

typedef http::server<WebServer::handler> http_server;

struct WebServer::handler {
	WebServer& m_server;

	void operator() (http_server::request const &request, http_server::response &response) {
		// Destination describes the file to be loaded, by default it's "/"
		if (request.method == "GET") {
			response = m_server.GETresponse(request);
		}
		else if (request.method == "POST") {
			response = m_server.POSTresponse(request);
		}
		else {
			response = http_server::response::stock_reply(
				http_server::response::ok, "other request");
		}
	}

	void log(http_server::string_type const &info) {
		LOG->Trace("webserver/error: %s", info );
	}
};


void WebServer::StartServer() {
	handler handler_{ *this };
	//http_server::options options(handler_);
	
	//m_server.reset(new http_server(options.address("0.0.0.0").port(81)));
	LOG->Trace("webserver/notice: Starting public server.");
	
	
	m_server->run();
}

WebServer::WebServer() {
	m_serverThread.reset(new boost::thread(boost::bind(&WebServer::StartServer, boost::ref(*this))));
}

WebServer::~WebServer() {
	m_server->stop();
	m_serverThread->join();
}

http_server::response WebServer::GETresponse(const http_server::request &request) {
	return http_server::response::stock_reply(http_server::response::ok, "nothing to see here");
	
}

http_server::response WebServer::POSTresponse(const http_server::request &request) {
	return http_server::response::stock_reply(http_server::response::ok, "nothing to see here");
}

/*
* (c) 2016- Electromuis, Anton Grootes
* This branch of https://github.com/stepmania/stepmania
* will from here on out be released as GPL v3 (wich converts from the previous MIT license)
*/

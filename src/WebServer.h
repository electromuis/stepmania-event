#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <boost/network/protocol/http/server.hpp>
namespace http = boost::network::http;

using boost::thread;

class WebServer
{
public:
	struct handler;
	typedef http::server<handler> http_server;
	typedef std::vector<std::uint8_t> BinaryBuffer;
	WebServer();
	~WebServer();
	// This looks silly but this stuff has to be public or they won't be accessible from the handler struct
	http_server::response GETresponse(http_server::request const &request);
	http_server::response POSTresponse(http_server::request const &request);

private:
	void StartServer();
	boost::shared_ptr<boost::thread> m_serverThread;
	boost::shared_ptr<http_server> m_server;
};

extern WebServer*	WEBSERVER;	// global and accessible from anywhere in our program

#endif
#pragma once

#include "WebsocketPlugin.h"

#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_TYPE_TRAITS_

#include <map>
#include <functional>

#include <websocketpp/server.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
using namespace websocketpp;

typedef websocketpp::server<websocketpp::config::asio> ws_server;
typedef websocketpp::client<websocketpp::config::asio> ws_client;

class WebSocketHandler {
public:
	WebSocketHandler(WebSocketPlugin* plugin);
	~WebSocketHandler();

	void RegisterFunction(RString name, SocketFunction function);

	static int StartWSThread(void* p);
	void WSThreadFunc();

	static int StartUDPThread(void* p);
	void UDPThreadFunc();

	void on_open(connection_hdl hdl);
	void on_close(connection_hdl hdl);
	void on_http(connection_hdl hdl);
	void on_message(connection_hdl hdl, ws_server::message_ptr msg);

	bool ReadUDPBroadcast();
	bool SendUDPBroadcast(Json::Value message);

	void GetSongsInGroup(RString group, Json::Value& response);
	void GetGroups(Json::Value& response);
	bool SelectSong(RString group, RString song);
	bool RunLua(Json::Value& req, Json::Value& res);

	bool IsWaiting() { return isWaiting; }
	void SetWaiting(bool waiting) { isWaiting = waiting; }

private:
	bool SetupUDP();

	ws_server m_server;
	std::vector<std::unique_ptr<ws_client>> m_cabs;

	WebSocketPlugin* m_plugin;

	RageThread m_wsThread;
	RageThread m_udpThread;

	bool isWaiting = false;
	bool stopRequested = false;
	std::string docRoot = "";
	std::string localIp = "";

	int udpSocket = -1;
};

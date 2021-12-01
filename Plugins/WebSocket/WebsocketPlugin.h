#pragma once

#include "config.hpp"
#include "global.h"
#include "PluginManager.h"
#include "MessageManager.h"
#include "RageThreads.h"
#include "JsonUtil.h"

#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_TYPE_TRAITS_

#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

using namespace websocketpp;
typedef websocketpp::server<websocketpp::config::asio> ws_server;


#define PLUGIN_NAME "WebsocketPlugin"

class WebSocketPlugin;

class WebSocketHandler {
public:
	WebSocketHandler(WebSocketPlugin* plugin);
	~WebSocketHandler();

	void SetupThread();
	static int StartThread(void* p);
	void ThreadFunc();

	void on_open(connection_hdl hdl);
	void on_close(connection_hdl hdl);
	void on_http(connection_hdl hdl);
	void on_message(connection_hdl hdl, ws_server::message_ptr msg);

	void GetSongsInGroup(RString group, Json::Value& response);
	void GetGroups(Json::Value &response);
	bool SelectSong(RString group, RString song);

private:
	ws_server m_server;
	WebSocketPlugin* m_plugin;
	RageThread m_thread;
	bool stopRequested = false;
	RString docRoot = "";
};

class PluginMessageSubscriber : public MessageSubscriber {
public:
	PluginMessageSubscriber(WebSocketPlugin* plugin);
	~PluginMessageSubscriber();

	void HandleMessage(const Message& msg);
private:
	WebSocketPlugin* plugin;
};

class WebSocketPlugin : public PluginBase {
public:
	WebSocketPlugin(std::string libraryPath);
	~WebSocketPlugin();

	void Update(float fDeltaTime);

	void Stepped();

	RString GetDocRoot();
private:
	std::string libraryPath;
	PluginMessageSubscriber subscriber;
	WebSocketHandler handler;
};

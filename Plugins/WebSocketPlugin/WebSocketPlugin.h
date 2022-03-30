#pragma once

#include "config.hpp"
#include "global.h"
#include "PluginManager.h"
#include "MessageManager.h"
#include "RageThreads.h"
#include "JsonUtil.h"

#ifdef _WIN32
#    ifdef WS_PL_IMPORT
#        define WS_PL_EXP __declspec(dllexport)
#    else
#        define WS_PL_EXP __declspec(dllimport)
#    endif
#elif
#    define LIBRARY_API
#endif

struct SocketRequest
{
	Json::Value request;
	Json::Value response;

	bool HandleRequest(std::string requestString);

	WS_PL_EXP void SetResponseField(RString key, bool value);
	WS_PL_EXP void SetResponseField(RString key, int value);
	WS_PL_EXP void SetResponseField(RString key, RString value);
};

typedef std::function<bool(SocketRequest* req)> SocketFunction;

class WebSocketPlugin;

#ifndef WS_PL_IMPORT
class PluginMessageSubscriber : public MessageSubscriber {
public:
	PluginMessageSubscriber(WebSocketPlugin* plugin);
	~PluginMessageSubscriber();

	void HandleMessage(const Message& msg);
private:
	WebSocketPlugin* plugin;
};
#endif

class WebSocketPlugin : public PluginBase {
public:
	WebSocketPlugin(std::string libraryPath);
	~WebSocketPlugin();

	WS_PL_EXP void RegisterFunction(RString name, SocketFunction function);
	void Update(float fDeltaTime);

	RString GetDocRoot();
private:
	std::string libraryPath;
	RString lastScreen = "";
#ifndef WS_PL_IMPORT
	PluginMessageSubscriber subscriber;
#endif
};

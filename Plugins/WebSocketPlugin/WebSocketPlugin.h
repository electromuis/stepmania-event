#pragma once

#include <functional>

#include "config.hpp"
#include "global.h"
#include "PluginManager.h"
#include "MessageManager.h"
#include "RageThreads.h"
#include "JsonUtil.h"

struct SocketRequest
{
	Json::Value request;
	Json::Value response;

	bool HandleRequest(std::string requestString);

	void SetResponseField(RString key, bool value);
	void SetResponseField(RString key, int value);
	void SetResponseField(RString key, RString value);
};

typedef std::function<bool(SocketRequest* req)> SocketFunction;

class WebSocketPlugin;

class WebSocketMessageSubscriber : public MessageSubscriber {
public:
	WebSocketMessageSubscriber(WebSocketPlugin* plugin);
	~WebSocketMessageSubscriber();

	void HandleMessage(const Message& msg);
private:
	WebSocketPlugin* plugin;
};

class WebSocketPlugin : public PluginBase {
public:
	WebSocketPlugin();
	~WebSocketPlugin();

	void RegisterFunctionInternal(RString name, SocketFunction function);
	void Update(float fDeltaTime);
	void* GetSymbol(const char* name);

	RString GetDocRoot();
private:
	std::string libraryPath;
	RString lastScreen = "";
	WebSocketMessageSubscriber subscriber;
};

typedef bool(*RegisterFunctionFunc)(PluginBase* plugin, RString name, SocketFunction function);
bool RegisterFunction(PluginBase* plugin, RString name, SocketFunction function);

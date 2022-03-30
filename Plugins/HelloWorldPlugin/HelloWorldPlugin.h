#pragma once

#include "config.hpp"
#include "global.h"
#include "PluginManager.h"
#include "MessageManager.h"

class HelloWorldPlugin;

class PluginMessageSubscriber : public MessageSubscriber {
public:
	PluginMessageSubscriber(HelloWorldPlugin* plugin);
	~PluginMessageSubscriber();

	void HandleMessage(const Message& msg);
private:
	HelloWorldPlugin* plugin;
};

class HelloWorldPlugin : public PluginBase {
public:
	HelloWorldPlugin(std::string libraryPath);
	~HelloWorldPlugin();

	void Update(float fDeltaTime);

	void Stepped();
private:
	PluginMessageSubscriber subscriber;
};

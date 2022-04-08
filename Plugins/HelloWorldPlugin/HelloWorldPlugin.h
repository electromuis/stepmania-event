#pragma once

#include "config.hpp"
#include "global.h"
#include "PluginManager.h"
#include "MessageManager.h"

class HelloWorldPlugin;

class HelloWorldMessageSubscriber : public MessageSubscriber {
public:
	HelloWorldMessageSubscriber(HelloWorldPlugin* plugin);
	~HelloWorldMessageSubscriber();

	void HandleMessage(const Message& msg);
private:
	HelloWorldPlugin* plugin;
};

class HelloWorldPlugin : public PluginBase {
public:
	HelloWorldPlugin();
	~HelloWorldPlugin();

	void Update(float fDeltaTime);

	void Stepped();
private:
	HelloWorldMessageSubscriber subscriber;
};

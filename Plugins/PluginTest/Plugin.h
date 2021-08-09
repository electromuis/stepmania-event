#pragma once

#include "config.hpp"
#include "global.h"
#include "PluginManager.h"
#include "MessageManager.h"

class PluginTest;

class PluginMessageSubscriber : public MessageSubscriber {
public:
	PluginMessageSubscriber(PluginTest* plugin);
	~PluginMessageSubscriber();

	void HandleMessage(const Message& msg);
private:
	PluginTest* plugin;
};

class PluginTest : public PluginBase {
public:
	PluginTest();
	~PluginTest();

	void Update(float fDeltaTime);

	void Stepped();
private:
	PluginMessageSubscriber subscriber;
};

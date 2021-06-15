#pragma once

#include <iostream>
using namespace std;

#include "global.h"
#include "Plugin.h"

#include "RageLog.h"

REGISTER_PLUGIN(PluginTest, "Test Plugin", "0.1.1")

PluginTest::PluginTest()
	:subscriber(this)
{	
	subscriber.SubscribeToMessage("Judgment");
	LOG->Info("Test plugin loaded");
}

PluginTest::~PluginTest()
{
	LOG->Info("Test plugin unloaded");
}

void PluginTest::Stepped()
{
	LOG->Info("Test plugin feels a step");
}

PluginMessageSubscriber::PluginMessageSubscriber(PluginTest* plugin)
	:plugin(plugin)
{

}

PluginMessageSubscriber::~PluginMessageSubscriber()
{
	UnsubscribeAll();
}

void PluginMessageSubscriber::HandleMessage(const Message& msg)
{
	plugin->Stepped();
}

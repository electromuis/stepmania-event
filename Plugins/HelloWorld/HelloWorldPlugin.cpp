#pragma once

#include "global.h"
#include "HelloWorldPlugin.h"

#include "RageLog.h"

REGISTER_PLUGIN(HelloWorldPlugin, PLUGIN_NAME, "0.0.1")

HelloWorldPlugin::HelloWorldPlugin(std::string libraryPath)
	:subscriber(this)
{	
	subscriber.SubscribeToMessage("Judgment");
	LOG->Info(PLUGIN_NAME" loaded");
}

HelloWorldPlugin::~HelloWorldPlugin()
{
	LOG->Info(PLUGIN_NAME" unloaded");
}

void HelloWorldPlugin::Stepped()
{
	LOG->Info(PLUGIN_NAME": Test plugin feels a step");
}

void HelloWorldPlugin::Update(float fDeltaTime)
{

}

PluginMessageSubscriber::PluginMessageSubscriber(HelloWorldPlugin* plugin)
	:plugin(plugin)
{
	SubscribeToMessage("Step");
}

PluginMessageSubscriber::~PluginMessageSubscriber()
{
	UnsubscribeAll();
}

void PluginMessageSubscriber::HandleMessage(const Message& msg)
{
	plugin->Stepped();
}

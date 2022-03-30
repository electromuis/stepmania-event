#pragma once

#include "global.h"
#include "HelloWorldPlugin.h"

#include "RageLog.h"

REGISTER_PLUGIN(HelloWorldPlugin, PLUGIN_NAME, "0.0.1")

HelloWorldPlugin::HelloWorldPlugin(std::string libraryPath)
	:subscriber(this)
{	
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
	RString test;
	RString test2 = "3";
	RString* text = APP_PTR(RString);
	text->assign("Step");
	SubscribeToMessage(*text);
	PLUGINMAN->AppDelete(text);
}

PluginMessageSubscriber::~PluginMessageSubscriber()
{
	UnsubscribeAll();
}

void PluginMessageSubscriber::HandleMessage(const Message& msg)
{
	plugin->Stepped();
}

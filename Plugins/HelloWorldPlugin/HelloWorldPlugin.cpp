#pragma once

#include "global.h"
#include "HelloWorldPlugin.h"

#include "RageLog.h"

REGISTER_PLUGIN(HelloWorldPlugin, "0.0.1", "Electromuis")

HelloWorldPlugin::HelloWorldPlugin()
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

HelloWorldMessageSubscriber::HelloWorldMessageSubscriber(HelloWorldPlugin* plugin)
	:plugin(plugin)
{
	SubscribeToMessage("Step");
}

HelloWorldMessageSubscriber::~HelloWorldMessageSubscriber()
{
	UnsubscribeAll();
}

void HelloWorldMessageSubscriber::HandleMessage(const Message& msg)
{
	plugin->Stepped();
}

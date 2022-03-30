#pragma once

#include <iostream>
#include <fstream>
using namespace std;

#include "global.h"
#include "WebsocketPlugin.h"
#include "SocketHandler.h"

#include "RageLog.h"
#include "ScreenManager.h"

#define PLUGIN_NAME "WebsocketPlugin"
REGISTER_PLUGIN(WebSocketPlugin, PLUGIN_NAME, "0.0.1")

AutoScreenMessage(SM_SongChanged);

unique_ptr<WebSocketHandler> handler;

WebSocketPlugin::WebSocketPlugin(std::string libraryPath)
	:libraryPath(libraryPath), subscriber(this)
{
	handler = make_unique<WebSocketHandler>(this);
	LOG->Info(PLUGIN_NAME" loaded");
}

WebSocketPlugin::~WebSocketPlugin()
{
	delete handler.release();
	LOG->Info(PLUGIN_NAME" unloaded");
}

void WebSocketPlugin::RegisterFunction(RString name, SocketFunction function)
{
	handler->RegisterFunction(name, function);
}

RString WebSocketPlugin::GetDocRoot()
{
	return "";
	/*
	RString lib;

	PLUGINMAN->CopyString(
		GetFileNameWithoutExtension(
			PLUGINMAN->MakeString(libraryPath.c_str())
		)
	);

	return "";

	RString folder, junk;
	splitpath(libraryPath, folder, junk, junk);

	if (lib && folder) {
		return folder + "/" + lib;
	}

	return "";
	*/
}

void WebSocketPlugin::Update(float fDeltaTime)
{
	Screen* screen = SCREENMAN->GetTopScreen();
	if (screen)
	{
		RString screenName = screen->GetName();
		if (lastScreen != screenName)
		{

			lastScreen = screenName;
		}
	}

}

void SocketRequest::SetResponseField(RString key, bool value)
{
	response[key] = value;
}

void SocketRequest::SetResponseField(RString key, int value)
{
	response[key] = value;
}

void SocketRequest::SetResponseField(RString key, RString value)
{
	response[key] = value;
}

PluginMessageSubscriber::PluginMessageSubscriber(WebSocketPlugin* plugin)
	:plugin(plugin)
{
	SubscribeToMessage("DoneLoadingNextSong");
}

PluginMessageSubscriber::~PluginMessageSubscriber()
{
	UnsubscribeAll();
}

void PluginMessageSubscriber::HandleMessage(const Message& msg)
{
	if (handler)
	{
		while (handler->IsWaiting())
			;
	}
}

#pragma once

#include "config.hpp"
#include "global.h"

#include "arch/Plugin/PluginDriver.h"
#include "MessageManager.h"
#include "PlayerNumber.h"
#include "NoteData.h"
#include "Actor.h"

#include <curl/curl.h>
#include "json/value.h"

class GrooveStatsPlugin;
class GrooveStatsClient;

class PluginMessageSubscriber : public MessageSubscriber {
public:
	PluginMessageSubscriber(GrooveStatsPlugin* plugin);
	~PluginMessageSubscriber();

	void HandleMessage(const Message& msg);
	void OnStatsUpdate();

private:
	GrooveStatsPlugin* plugin;
	RString lastScreen;
	Actor* screenListner = nullptr;
};

class GrooveStatsPlugin : public PluginBase {
public:
	GrooveStatsPlugin(std::string libraryPath);
	~GrooveStatsPlugin();

	virtual std::vector<PluginRegType>* GrooveStatsPlugin::GetLuaFunctions();

	void Update(float fDeltaTime);
private:
	PluginMessageSubscriber subscriber;
};

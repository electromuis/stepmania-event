#pragma once

#include "config.hpp"
#include "global.h"

#include "arch/Plugin/PluginDriver.h"
#include "MessageManager.h"
#include "PlayerNumber.h"
#include "NoteData.h"
#include "Actor.h"

#include <curl/curl.h>

class GrooveStatsPlugin;
class GrooveStatsClient;

class GrooveStatsMessageSubscriber : public MessageSubscriber {
public:
	GrooveStatsMessageSubscriber(GrooveStatsPlugin* plugin);
	~GrooveStatsMessageSubscriber();

	void HandleMessage(const Message& msg);
	void OnStatsUpdate();

private:
	GrooveStatsPlugin* plugin;
	RString lastScreen;
	Actor* screenListner = nullptr;
};

class GrooveStatsPlugin : public PluginBase {
public:
	GrooveStatsPlugin();
	~GrooveStatsPlugin();

	virtual std::vector<PluginRegType>* GetLuaFunctions();

	void Update(float fDeltaTime);
private:
	GrooveStatsMessageSubscriber subscriber;
};

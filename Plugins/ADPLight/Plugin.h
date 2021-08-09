#pragma once

#include "config.hpp"
#include "global.h"
#include "PluginManager.h"
#include "MessageManager.h"
#include "PlayerNumber.h"

class ADPLights;

struct InputLightRule
{
	int colNr = -1;
	int gameBtn;
	int lightRuleIndex;
	int lightMappingIndex;
	int sensorNr;
};

class PluginMessageSubscriber : public MessageSubscriber {
public:
	PluginMessageSubscriber(ADPLights* plugin);
	~PluginMessageSubscriber();

	void HandleMessage(const Message& msg);
private:
	ADPLights* plugin;
};

class ADPLights : public PluginBase {
public:
	ADPLights();
	~ADPLights();

	virtual void Update(float fDeltaTime);

	void Stepped(PlayerNumber player, TapNoteScore score, int col);
	void UpdateLightMap();

private:
	PluginMessageSubscriber subscriber;
	float updateWaiter = 0;
	InputLightRule lightMap[100];
};

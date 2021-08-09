#pragma once

#include <iostream>
#include "global.h"
#include "Plugin.h"
#include "RageLog.h"
#include "InputMapper.h"
#include "Style.h"
#include "GameState.h"
#include "ScreenManager.h"
#include "Model/Device.h"
#include "Model/Log.h"

using namespace std;
using namespace adp;

REGISTER_PLUGIN(ADPLights, "ADPLights", "0.0.1")

ADPLights::ADPLights()
	:subscriber(this)
	,lightMap()
{
	Device::Init();
	Log::Init();
	Device::Update();

	if (Device::Pad() != nullptr) {
		Device::SetLightMode(LM_SOFTWARE);
	}


	//UpdateLightMap();

	subscriber.SubscribeToMessage("Judgment");
	subscriber.SubscribeToMessage("ScreenChanged");

	LOG->Info("ADPLights loaded");
}

ADPLights::~ADPLights()
{
	if (Device::Pad() != nullptr) {
		Device::SetLightMode(LM_HARDWARE);
	}

	Device::Shutdown();
	Log::Shutdown();

	LOG->Info("ADPLights unloaded");
}

void ADPLights::UpdateLightMap()
{
	for (int i = 0; i < 100; i++) {
		this->lightMap[i].colNr = -1;
	}

	const Style* style = GAMESTATE->GetCurrentStyle(PLAYER_1);
	if (style == nullptr) {
		return;
	}

	auto pad = Device::Pad();
	auto lights = Device::Lights();
	if (pad == nullptr || lights == nullptr) {
		return;
	}

	int columnButtonMap[NUM_GameButton];
	memcpy(&columnButtonMap, style->m_iInputColumn[GameController_1], sizeof(int) * NUM_GameButton);

	int c = 0;

	for (int col = 0; col < style->m_iColsPerPlayer; col++) {
		if (c > 99) {
			return;
		}

		GameInput GameI;
		GameI.button = (GameButton)(GAME_BUTTON_NEXT + columnButtonMap[col]);
		GameI.controller = GameController_1;

		DeviceInput DeviceI;
		if (!INPUTMAPPER->GameToDevice(GameI, 0, DeviceI)) {
			continue;
		}

		int inputNumber = DeviceI.button - JOY_BUTTON_1 + 1;
		
		for (int s = 0; s < pad->numSensors; s++) {
			auto padSensor = Device::Sensor(s);
			if (padSensor == nullptr) {
				continue;
			}

			if (padSensor->button == inputNumber) {
				auto it = lights->ledMappings.begin();

				while (it != lights->ledMappings.end())
				{
					if (it->second.sensorIndex == s) {
						lightMap[c].colNr = col;
						lightMap[c].gameBtn = inputNumber;
						lightMap[c].lightRuleIndex = it->second.lightRuleIndex;
						lightMap[c].lightMappingIndex = it->first;
						lightMap[c].sensorNr = it->second.sensorIndex;

						c++;
					}

					it++;
				}
			}
		}
	}
}

void ADPLights::Stepped(PlayerNumber player, TapNoteScore score, int col)
{
	LOG->Info("ADPLights feels a step");

	LightRule rule;
	rule.offColor = { 0,0,0 };
	rule.fadeOff = false;
	rule.fadeOn = false;
	rule.pulseOn = true;
	rule.pulseOff = false;

	bool setColor = false;

	if (score == TNS_W1) {
		rule.onColor = { 3, 207, 252 };
		setColor = true;
	}
	else if (score == TNS_W2) {
		rule.onColor = { 232, 252, 3 };
		setColor = true;
	}
	else if(score == TNS_W3) {
		rule.onColor = { 0, 255, 0 };
		setColor = true;
	}
	else {
		rule.onColor = { 255, 0, 0 };
		setColor = true;
	}

	if (setColor) {
		for (int i = 0; i < 100; i++)
		{
			if (lightMap[i].colNr == -1) {
				return;
			}

			if (lightMap[i].colNr != col) {
				continue;
			}

			Device::SendLightRule(lightMap[i].lightRuleIndex, rule);
			Device::TriggerLight(lightMap[i].lightMappingIndex);			
		}
	}
}

void ADPLights::Update(float fDeltaTime)
{
	updateWaiter += fDeltaTime;

	if (updateWaiter > 0.1) {

		auto changes = Device::Update();
		if (changes & DCF_DEVICE) {
			LOG->Info("ADPLights: device change");

			if (Device::Pad() != nullptr) {
				Device::SetLightMode(LM_SOFTWARE);
			}
		}

		updateWaiter = 0;
	}
}

PluginMessageSubscriber::PluginMessageSubscriber(ADPLights* plugin)
	:plugin(plugin)
{

}

PluginMessageSubscriber::~PluginMessageSubscriber()
{
	UnsubscribeAll();
}

void PluginMessageSubscriber::HandleMessage(const Message& msg)
{
	if (msg.GetName() == "Judgment")
	{
		TapNoteScore score;
		int iTrack;
		PlayerNumber player;

		msg.GetParam("TapNoteScore", score);
		msg.GetParam("FirstTrack", iTrack);
		msg.GetParam("Player", player);

		plugin->Stepped(player, score, iTrack);
	}
	else if (msg.GetName() == "ScreenChanged") {
		plugin->UpdateLightMap();
	}
}

#pragma once

#include "global.h"
#include "JsonUtil.h"

#include "GrooveStatsPlugin.h"
#include "GrooveStatsClient.h"

#include "GameState.h"
#include "StatsManager.h"
#include "ScreenManager.h"
#include "RageFileManager.h"

#include "Actor.h"
#include "Screen.h"
#include "ScreenGameplay.h"

#include "Player.h"
#include "PlayerNumber.h"

REGISTER_PLUGIN(GrooveStatsPlugin, "0.0.1")

GrooveStatsPlugin::GrooveStatsPlugin()
	:subscriber(this)
{
	GROOVESTATS_CLIENT.Initialize("https://api.groovestats.com");
	
	LOG->Info(PLUGIN_NAME" loaded");
}

GrooveStatsPlugin::~GrooveStatsPlugin()
{
	LOG->Info(PLUGIN_NAME" unloaded");
}

void GrooveStatsPlugin::Update(float fDeltaTime)
{
	vector<RString>* files = APP_PTR(vector<RString>);
	FILEMAN->GetDirListing("Save/GrooveStats/requests/*.json", *files, false, false);

	for (auto& f : *files)
	{
		Json::Value* root = APP_PTR(Json::Value);
		if (!JsonUtil::LoadFromFileShowErrors(*root, "Save/GrooveStats/requests/" + f))
			bool f = true;

		Json::Value output;
		if (!GROOVESTATS_CLIENT.HandleRequest(*root, output["data"]))
			output["status"] = "fail";
		else
			output["status"] = "success";

		
		JsonUtil::WriteFile(output, "Save/GrooveStats/responses/" + f, true);
		if (!FILEMAN->Remove("Save/GrooveStats/requests/" + f))
			bool f = true;

		PLUGINMAN->AppDelete(root);
	}


	PLUGINMAN->AppDelete(files);
}

class ScreenListner : public Actor {
public:
	ScreenListner(GrooveStatsMessageSubscriber* messageSubscriber)
		:subscriber(messageSubscriber)
	{
		SetName("ScreenListener");
		LOG->Info("ScreenListener here");

		
	}

	void HandleMessage(const Message& msg)
	{
		subscriber->HandleMessage(msg);
		LOG->Info("ScreenListener: %s", msg.GetName().c_str());
	}
private:
	GrooveStatsMessageSubscriber* subscriber;
};

GrooveStatsMessageSubscriber::GrooveStatsMessageSubscriber(GrooveStatsPlugin* plugin)
	:plugin(plugin)
{
	SubscribeToMessage("PingLauncher");
	SubscribeToMessage("NewSessionRequest");

	SubscribeToMessage("ScreenChanged");
	//SubscribeToMessage("Step");
	//SubscribeToMessage("Judgment");
	//SubscribeToMessage("ColumnJudgment");
}

GrooveStatsMessageSubscriber::~GrooveStatsMessageSubscriber()
{
	UnsubscribeAll();
}

void GrooveStatsMessageSubscriber::HandleMessage(const Message& msg)
{
	if (msg.GetName() == "PingLauncher")
	{
		return;
		LuaReference callback;
		if (msg.GetParam("callback", callback))
		{
			bool f = true;

			Lua* L = LUA->Get();
			callback.PushSelf(L);

			// Response
			LuaHelpers::Push(L, true);

			// Response args
			LuaHelpers::Push(L, true);

			RString err = "Error running MarginFunction:  ";
			if (LuaHelpers::RunScriptOnStack(L, err, 2, 0, true))
			{
				bool s = true;
			}
			else
			{
				bool err = true;
			}
			

			LUA->Release(L);
		}
		

		return;
	}

	else if (msg.GetName() == "NewSessionRequest")
	{
		bool s = true;
	}

	Screen* screen = SCREENMAN->GetTopScreen();
	if (screen == nullptr)
		return;
	RString screenName = screen->GetName();

	if (msg.GetName() == "ScreenChanged")
	{
		if (
			lastScreen.find("Gameplay") != std::string::npos &&
			screenName.find("Evaluation") != std::string::npos
		) {
			// Post score
			
		}
		
		if (screenName.find("Gameplay") != std::string::npos) {
			if (screenListner != nullptr)
				delete screenListner;
			screenListner = new ScreenListner(this);
			screen->AddChild(screenListner);
		}

		lastScreen = screenName;
	}
}


std::vector<PluginRegType>* GrooveStatsPlugin::GetLuaFunctions()
{
	std::vector<PluginRegType>* out = new std::vector<PluginRegType>();

	

	return out;
}

#pragma once

#include "config.hpp"
#include "global.h"

#include "arch/Plugin/PluginDriver.h"
#include "MessageManager.h"
#include "PlayerNumber.h"
#include "NoteData.h"
#include "Actor.h"
#include "PadmissClient.h"

#include <curl/curl.h>
#include "json/value.h"

#define PLUGIN_NAME "Padmiss Plugin"

class PadmissPlugin;
class PadmissClient;

struct PlayerInputEvent
{
	/** @brief Beat position in the song where the input event happened */
	float beat;

	/** @brief Column index of the target arrow */
	int column;
	
	/** @brief Was the input released */
	bool released;
};

class PluginMessageSubscriber : public MessageSubscriber {
public:
	PluginMessageSubscriber(PadmissPlugin* plugin);
	~PluginMessageSubscriber();

	void HandleMessage(const Message& msg);
	void OnStatsUpdate();
	void InputEvent(DeviceInput di, GameInput gi, int col);

private:
	PadmissPlugin* plugin;
	RString lastScreen;
	Actor* screenListner = nullptr;
	NoteData lastNoteData[NUM_PLAYERS];
	vector<PlayerInputEvent> lastInputEvents[NUM_PLAYERS];
};

class PadmissPlugin : public PluginBase {
public:
	PadmissPlugin(std::string libraryPath);
	~PadmissPlugin();

	bool HasScreen(const char* sName);
	void PluginFreeScreen(void* p);
	void PluginFree(void* p);
	void PluginDelete(void* p);
	bool Login(RString username, RString password) { return client->Login(username, password); };
	bool IsLoggedIn() { return client->IsLoggedIn(); };
	bool DownloadProfile();

	virtual std::vector<PluginRegType>* PadmissPlugin::GetLuaFunctions();

	void Update(float fDeltaTime);
	void StatsUpdateV1(StageStats* pSS, PlayerNumber pn);
	void StatsUpdateV2(StageStats* pSS, const NoteData& noteData, const std::vector<PlayerInputEvent>& inputEvents, PlayerNumber pn);
	bool IsWorking();
	void SetTrackInputs(bool trackInputs);
private:
	PluginMessageSubscriber subscriber;
	PadmissClient* client;
	bool trackInputs = false;
	bool lastInputStatus[NUM_GameController][MAX_NOTE_TRACKS] = { false };
};

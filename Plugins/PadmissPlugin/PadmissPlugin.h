#pragma once

#include "config.hpp"
#include "global.h"

#include "arch/Plugin/PluginDriver.h"
#include "MessageManager.h"
#include "PlayerNumber.h"
#include "NoteData.h"
#include "Actor.h"
#include "RageInputDevice.h"
#include "GameInput.h"
#include "RageFileDriverCmrc.h"

#include "PadmissClient.h"
#include "ProfileState.h"

#include <curl/curl.h>

#define PLUGIN_NAME "PadmissPlugin"

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

class PadmissMessageSubscriber : public MessageSubscriber {
public:
	PadmissMessageSubscriber(PadmissPlugin* plugin);
	~PadmissMessageSubscriber();

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
	PadmissPlugin();
	~PadmissPlugin();

	void UnpackResources(cmrc::embedded_filesystem fs, string parent);
	bool HasScreen(const char* sName);
	void PluginFreeScreen(void* p);
	void PluginFree(void* p);
	void PluginDelete(void* p);
	bool Login(RString username, RString password) { return PADMISS_CLIENT.Login(username, password); };
	bool IsLoggedIn() { return PADMISS_CLIENT.IsLoggedIn(); };
	bool DownloadProfile(RString username);

	virtual std::vector<PluginRegType>* GetLuaFunctions();

	void Update(float fDeltaTime);
	void StatsUpdateV1(StageStats* pSS, PlayerNumber pn);
	void StatsUpdateV2(StageStats* pSS, const NoteData& noteData, const std::vector<PlayerInputEvent>& inputEvents, PlayerNumber pn);
	bool IsWorking();
	void SetTrackInputs(bool trackInputs);
private:
	bool HookSocket();
	PadmissMessageSubscriber subscriber;
	map<int, string> chartMap;
	bool trackInputs = false;
	bool lastInputStatus[NUM_GameController][MAX_NOTE_TRACKS] = { false };
	bool loaded = false;
	static ProfileState profileStates[NUM_PLAYERS];
	RageFileDriverCmrc fs;
};

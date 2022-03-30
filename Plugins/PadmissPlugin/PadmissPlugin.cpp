#pragma once

#include <stdio.h>
#include "json/writer.h"
#include "json/reader.h"

#include "global.h"

#include "PadmissPlugin.h"
#include "PadmissClient.h"
#include "Reader.h"
#include "ScreenPadmiss.h"
#include "NotesWriterPadmiss.h"

//#include "ScreenOptionsManageProfilesPM.h"
#include "NotesWriterSM.h"
#include "RageFileDriverMemory.h"

#include "GameState.h"
#include "StatsManager.h"
#include "ScreenManager.h"
#include "InputMapper.h"
#include "ProfileManager.h"
#include "GameInput.h"
#include "InputFilter.h"
#include "PrefsManager.h"
#include "SongManager.h"

#include "Actor.h"
#include "Screen.h"
#include "ScreenGameplay.h"
#include "ScreenTextEntry.h"

#include "Player.h"
#include "PlayerNumber.h"
#include "PlayerState.h"
#include "Steps.h"
#include "Song.h"
#include "NoteTypes.h"

#define WS_PL_IMPORT
#include "WebSocketPlugin.h"

const RString GROUP_ALL = "---Group All---";


REGISTER_PLUGIN(PadmissPlugin, PLUGIN_NAME, "0.0.1")

Screen* screen = nullptr;
ProfileState PadmissPlugin::profileStates[NUM_PLAYERS] = {
	ProfileState(0),
	ProfileState(1)
};

DEFINE_SCREEN_CLASS(ScreenPadmiss)

PadmissPlugin::PadmissPlugin(std::string libraryPath)
	:subscriber(this)
{
	PADMISS_CLIENT.Initialize("https://api.padmiss.com");
	
	//FOREACH_PlayerNumber(pn)
		//profileStates[pn].Init();
	
	LOAD_SCREEN_CLASS(ScreenPadmiss);
	
	/*
	auto songs = SONGMAN->GetAllSongs();
	for (Song* song : songs)
	{
		for (Steps* steps : song->GetAllSteps())
		{
			int smHash = steps->GetHash();
			if (chartMap.count(smHash) == 0)
			{
				string pmHash = PADMISS_CLIENT.ChartHash(steps, song);
				if (pmHash.size() > 0)
				{
					chartMap.insert(pair<int, string>(smHash, pmHash));
				}
				else
				{
					bool f = true;
				}
			}
		}
	}
	*/

	
	LOG->Info(PLUGIN_NAME" loaded");
}

PadmissPlugin::~PadmissPlugin()
{
	LOG->Info(PLUGIN_NAME" unloaded");
}

bool PadmissPlugin::HasScreen(const char* sName)
{
	if (strcmp(sName, "ScreenPadmiss") == 0) {
		return true;
	}

	return false;
}

void PadmissPlugin::PluginFree(void* p)
{
	free(p);
}

void PadmissPlugin::PluginDelete(void* p)
{
	
}

void PadmissPlugin::Update(float fDeltaTime)
{
	return;

	if (!loaded)
	{
		auto ws = PLUGINMAN->GetPlugin("WebsocketPlugin");
		if (ws)
		{
			WebSocketPlugin* wsp = (WebSocketPlugin*)ws->GetPlugin();
			if (wsp)
			{
				auto me = this;

				wsp->RegisterFunction("Login", [me](SocketRequest* req) {
					if (!req->request.isMember("playerNumber") || !req->request["playerNumber"].isInt())
						return false;

					if (!req->request.isMember("playerId") || !req->request["playerId"].isString())
						return false;

					int pn = req->request["playerNumber"].asInt();
					if (pn < 0 || pn >= NUM_PLAYERS)
						return false;

					me->profileStates[pn].SignIn(req->request["playerId"].asString());
				
//					req->SetResponseField("message", 123);
					

					return true;
				});

				wsp->RegisterFunction("Logout", [me](SocketRequest* req) {
					if (!req->request.isMember("playerNumber") || !req->request["playerNumber"].isInt())
						return false;

					int pn = req->request["playerNumber"].asInt();
					if (pn < 0 || pn >= NUM_PLAYERS)
						return false;

					me->profileStates[pn].SignOut();

					return true;
				});


				loaded = true;
			}
		}
	}

	if (trackInputs) {
		void* deviceInputSpace = PLUGINMAN->AppAllocate(sizeof(vector<DeviceInput>));
		vector<DeviceInput>* DeviceInputs = new(deviceInputSpace) vector<DeviceInput>();

		INPUTFILTER->GetPressedButtons(*DeviceInputs);
		for (DeviceInput const& di : *DeviceInputs)
		{
			void* gameInputSpace = PLUGINMAN->AppAllocate(sizeof(GameInput));
			GameInput* gi = new(gameInputSpace) GameInput();

			if (!INPUTMAPPER->DeviceToGame(di, *gi))
				continue;

			if (gi->controller == GameController_Invalid)
				continue;

			if (gi->button == GameButton_Invalid || gi->button < GAME_BUTTON_CUSTOM_01 || gi->button > GAME_BUTTON_CUSTOM_19)
				continue;

			int col = gi->button - GAME_BUTTON_CUSTOM_01;

			if (lastInputStatus[gi->controller][col] == di.bDown)
				continue;

			lastInputStatus[gi->controller][col] = di.bDown;

			subscriber.InputEvent(di, *gi, col);

			PLUGINMAN->AppFree(gi);
		}

		PLUGINMAN->AppDelete(DeviceInputs);
	}

	FOREACH_PlayerNumber(pn)
		profileStates[pn].Update();
}

void PadmissPlugin::SetTrackInputs(bool trackInputs)
{
	this->trackInputs = trackInputs;
}

void PadmissPlugin::StatsUpdateV1(StageStats* pSS, PlayerNumber pn)
{
	Json::Value request;

	Profile* profile = PROFILEMAN->GetProfile(pn);
	const PlayerStageStats* playerStats = &pSS->m_player[pn];
	const PlayerOptions& opts = GAMESTATE->m_pPlayerState[pn]->m_PlayerOptions.Get(ModsLevel_Preferred);

	if (profile->IsMachine() || !profile->m_UserTable.IsSet())
		return;

	Lua* L = LUA->Get();
	RString PadmissId;
	profile->m_UserTable.Get(L, "PadmissId");
	LuaHelpers::FromStack(L, PadmissId, 0);
	lua_pop(L, 1);
	LUA->Release(L);

	if (PadmissId.length() == 0)
		return;

	Steps* steps = playerStats->m_vpPossibleSteps[0]; // XXX Courses and such
	Song* song = steps->m_pSong;
	steps->Decompress(); // Hashing won't work unless the steps are decompressed

	RageFileObjMem f;
	vector<Steps*> stepv;
	stepv.push_back(steps);
	NotesWriterSM::Write(f, *song, stepv);

	request["hash"] = steps->GetHash();
	request["meter"] = steps->GetMeter();
	request["playMode"] = "Single";
	request["stepData"] = f.GetString();
	request["stepArtist"] = steps->GetCredit();
	request["title"] = song->m_sMainTitle;
	request["artist"] = song->m_sArtist;
	request["durationSeconds"] = (int)song->m_fMusicLengthSeconds;
	request["group"] = song->m_sGroupName;

	request["playerId"] = PadmissId;
	request["passed"] = !playerStats->m_bFailed;
	request["secondsSurvived"] = playerStats->m_fAliveSeconds;
	request["scoreValue"] = playerStats->GetPercentDancePoints();
	request["fantastics"] = playerStats->m_iTapNoteScores[TapNoteScore::TNS_W1];
	request["excellents"] = playerStats->m_iTapNoteScores[TapNoteScore::TNS_W2];
	request["greats"] = playerStats->m_iTapNoteScores[TapNoteScore::TNS_W3];
	request["decents"] = playerStats->m_iTapNoteScores[TapNoteScore::TNS_W4];
	request["wayoffs"] = playerStats->m_iTapNoteScores[TapNoteScore::TNS_W5];
	request["misses"] = playerStats->m_iTapNoteScores[TapNoteScore::TNS_Miss];

	request["holds"] = playerStats->m_radarActual[RadarCategory_Holds];
	request["holdsTotal"] = playerStats->m_radarPossible[RadarCategory_Holds];

	request["rolls"] = playerStats->m_radarActual[RadarCategory_Rolls];
	request["rollsTotal"] = playerStats->m_radarPossible[RadarCategory_Rolls];

	request["jumps"] = playerStats->m_radarActual[RadarCategory_Jumps];
	request["jumpsTotal"] = playerStats->m_radarPossible[RadarCategory_Jumps];

	request["minesHit"] = playerStats->m_iTapNoteScores[TapNoteScore::TNS_HitMine];
	request["minesAvoided"] = playerStats->m_iTapNoteScores[TapNoteScore::TNS_AvoidMine];
	request["minesTotal"] = playerStats->m_radarPossible[RadarCategory_Mines];	

	request["hands"] = playerStats->m_radarActual[RadarCategory_Hands];
	request["handsTotal"] = playerStats->m_radarPossible[RadarCategory_Hands];
	
	request["cabSide"] = "Left";
	request["speedMod"]["type"] = "Multiplier";
	request["speedMod"]["value"] = 1;
	request["musicRate"] = pSS->m_fMusicRate;
	request["noteSkin"] = opts.m_sNoteSkin;
	request["perspective"]["tilt"] = opts.m_fPerspectiveTilt;
	request["perspective"]["skew"] = opts.m_fSkew;

	request["timingWindows"]["fantasticTimingWindow"] = Player::GetWindowSeconds(TimingWindow::TW_W1);
	request["timingWindows"]["excellentTimingWindow"] = Player::GetWindowSeconds(TimingWindow::TW_W2);
	request["timingWindows"]["greatTimingWindow"] = Player::GetWindowSeconds(TimingWindow::TW_W3);
	request["timingWindows"]["decentTimingWindow"] = Player::GetWindowSeconds(TimingWindow::TW_W4);
	request["timingWindows"]["wayoffTimingWindow"] = Player::GetWindowSeconds(TimingWindow::TW_W5);
	request["timingWindows"]["mineTimingWindow"] = Player::GetWindowSeconds(TimingWindow::TW_Mine);
	request["timingWindows"]["holdTimingWindow"] = Player::GetWindowSeconds(TimingWindow::TW_Hold);
	request["timingWindows"]["rollTimingWindow"] = Player::GetWindowSeconds(TimingWindow::TW_Roll);

	/*
	request["inputEvents"] = 
	request["noteScoresWithBeats"]

	request["modsTurn"]
	request["modsTransform"]
	request["modsOther"]
	*/

	PADMISS_CLIENT.SendScoreV1(request);

	LOG->Info("PadmissPlugin: score sent");
}

void PadmissPlugin::StatsUpdateV2(StageStats* pSS, const NoteData& noteData, const std::vector<PlayerInputEvent>& inputEvents, PlayerNumber pn)
{
	Json::Value request;

	Profile* profile = PROFILEMAN->GetProfile(pn);
	const PlayerStageStats* playerStats = &pSS->m_player[pn];
	const PlayerOptions& opts = GAMESTATE->m_pPlayerState[pn]->m_PlayerOptions.Get(ModsLevel_Preferred);

	Steps* steps = playerStats->m_vpPossibleSteps[0]; // XXX Courses and such
	Song* song = steps->m_pSong;

	string chartHash = PADMISS_CLIENT.ChartHash(steps, song);
	if (chartHash.length() == 0) {
		LOG->Info("PadmissPlugin: checking chart failed");
		return;
	}

	Lua* L = LUA->Get();
	int PadmissId;
	profile->m_UserTable.Get(L, "PadmissId");
	LuaHelpers::FromStack(L, PadmissId, 0);
	lua_pop(L, 1);
	LUA->Release(L);

	if (PadmissId == 0)
		return;

	request["playerId"] = PadmissId;
	request["stepChartHash"] = chartHash;

	request["inputEvents"] = Json::Value(Json::arrayValue);
	request["noteScores"] = Json::Value(Json::arrayValue);

	request["noteSkin"] = opts.m_sNoteSkin;
	request["perspective"]["name"] = "overhead";
	request["perspective"]["tilt"] = opts.m_fPerspectiveTilt;;
	request["perspective"]["skew"] = opts.m_fSkew;
	request["speedMod"]["type"] = "MaxBPM";
	request["speedMod"]["value"] = 1;
	request["musicRate"] = pSS->m_fMusicRate;

	request["noteScores"] = Json::Value(Json::arrayValue);

	int noteIndex = 0;
	FOREACH_NONEMPTY_ROW_ALL_TRACKS(noteData, row) {
		float beat = NoteRowToBeat(row);

		for (int t = 0; t < noteData.GetNumTracks(); t++)
		{
			
			TapNote tapNote = noteData.GetTapNote(t, row);
			if (tapNote.type == TapNoteType_Empty)
				continue;

			request["noteScores"].resize(request["noteScores"].size() + 1);
			Json::Value& noteRow = request["noteScores"][request["noteScores"].size() - 1];

			noteRow["noteIndex"] = noteIndex;
			noteRow["beat"] = beat;
			noteRow["column"] = t;
			noteRow["holdNoteScore"] = tapNote.HoldResult.hns;
			noteRow["tapNoteScore"] = tapNote.result.tns;
			noteRow["offset"] = tapNote.result.fTapNoteOffset;
		}

		noteIndex++;
	}

	request["inputEvents"] = Json::Value(Json::arrayValue);
	for (PlayerInputEvent e : inputEvents)
	{
		request["inputEvents"].resize(request["inputEvents"].size() + 1);
		Json::Value& inputEvent = request["inputEvents"][request["inputEvents"].size() - 1];

		inputEvent["beat"] = e.beat;
		inputEvent["column"] = e.column;
		inputEvent["released"] = e.released;
	}

	PADMISS_CLIENT.SendScoreV2(request);

	LOG->Info("PadmissPlugin: score sent");
}

const RString USER_PROFILES_DIR = "/Save/LocalProfiles/";
static RString LocalProfileIDToDir(const RString& sProfileID) { return USER_PROFILES_DIR + sProfileID + "/"; }

bool PadmissPlugin::DownloadProfile(RString username)
{
	Json::Value data;
	if (!PADMISS_CLIENT.SendGraphql("{Players(where:{nickname:{eq:\"" + username + "\"}}) {nodes { id nickname }}}", data))
		return false;

	Json::Value& n = data["Players"]["nodes"];
	//n[int(0)].size();

	if (!data["Players"]["nodes"])
		return false;
	

	RString profileId;
	if (!PROFILEMAN->CreateLocalProfile(username, profileId))
		return false;

	Profile* profile = PROFILEMAN->GetLocalProfile(profileId);

	Lua* L = LUA->Get();

	lua_newtable(L);
	lua_pushstring(L, "PadmissId");
	lua_pushstring(L, data["Player"]["id"].asCString());
	lua_settable(L, -3);
	profile->m_UserTable.SetFromStack(L);
	lua_settop(L, 0);

	LUA->Release(L);

	PROFILEMAN->SaveLocalProfile(profileId);

	return true;
}

class ScreenListner : public Actor {
public:
	ScreenListner(PluginMessageSubscriber* messageSubscriber)
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
	PluginMessageSubscriber* subscriber;
};

PluginMessageSubscriber::PluginMessageSubscriber(PadmissPlugin* plugin)
	:plugin(plugin)
{
	//SubscribeToMessage("ScreenChanged");
	//SubscribeToMessage("Step");
	//SubscribeToMessage("Judgment");
	//SubscribeToMessage("ColumnJudgment");
}

PluginMessageSubscriber::~PluginMessageSubscriber()
{
	UnsubscribeAll();
}

void PluginMessageSubscriber::InputEvent(DeviceInput di, GameInput gi, int col)
{
	lastInputEvents[gi.controller].push_back({
		GAMESTATE->m_Position.m_fSongBeat,
		col,
		!di.bDown
	});
}

void PluginMessageSubscriber::HandleMessage(const Message& msg)
{
	Screen* screen = SCREENMAN->GetTopScreen();
	if (screen == nullptr)
		return;
	RString screenName = screen->GetName();

	if (msg.GetName() == "Off" && screenName.find("Gameplay") != std::string::npos)
	{
		ScreenGameplay* screenGameplay = (ScreenGameplay *)screen;
		FOREACH_HumanPlayer(pn)
		{
			PlayerInfo* pi = screenGameplay->GetPlayerInfo(pn);
			lastNoteData[pn] = pi->m_NoteData;
		}	
	}

	if (msg.GetName() == "ScreenChanged")
	{
		if (
			lastScreen.find("Gameplay") != std::string::npos &&
			screenName.find("Evaluation") != std::string::npos
		) {
			FOREACH_HumanPlayer(pn)
			{
				plugin->StatsUpdateV2(&STATSMAN->m_CurStageStats, lastNoteData[pn], lastInputEvents[pn], pn);
			}
		}
		
		if (screenName.find("Gameplay") != std::string::npos) {
			if (screenListner != nullptr)
				delete screenListner;
			screenListner = new ScreenListner(this);
			screen->AddChild(screenListner);

			FOREACH_PlayerNumber(pn)
				lastInputEvents[pn].clear();

			plugin->SetTrackInputs(true);
		}
		else {
			plugin->SetTrackInputs(false);
		}

		lastScreen = screenName;
		return;
	}
}

static int LuaIsWorking(LoadedPlugin* p, lua_State* L)
{
	PadmissPlugin* me = (PadmissPlugin*)p->GetPlugin();

	//lua_pushboolean(L, me->IsWorking());
	lua_pushboolean(L, false);

	return 1;
}

static int LuaLogin(LoadedPlugin* p, lua_State* L)
{
	PadmissPlugin* me = (PadmissPlugin*)p->GetPlugin();

	bool result = me->Login(SArg(1), SArg(2));
	lua_pushboolean(L, result);

	return 1;
}

static int LuaDownloadProfile(LoadedPlugin* p, lua_State* L)
{
	PadmissPlugin* me = (PadmissPlugin*)p->GetPlugin();

	ScreenTextEntry::TextEntry(SM_None, "Username?", "", 200, nullptr, [](const RString& answer) {
		PadmissPlugin* me = (PadmissPlugin*)PLUGINMAN->GetPlugin(PLUGIN_NAME)->GetPlugin();
		if (me->DownloadProfile(answer))
			SCREENMAN->SystemMessage("Downloaded");
		else
			SCREENMAN->SystemMessage("Failed");
	});

	//bool result = me->DownloadProfile();
	//lua_pushboolean(L, result);

	return 1;
}

std::vector<PluginRegType>* PadmissPlugin::GetLuaFunctions()
{
	std::vector<PluginRegType>* out = new std::vector<PluginRegType>();

	out->push_back({"IsWorking", LuaIsWorking});
	out->push_back({"Login", LuaLogin});
	out->push_back({"DownloadProfile", LuaDownloadProfile });

	return out;
}

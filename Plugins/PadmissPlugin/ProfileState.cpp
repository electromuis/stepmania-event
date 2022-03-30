#include "global.h"

#include "ProfileState.h"
#include "PadmissClient.h"
#include "ProfileManager.h"
#include "GameState.h"
#include "ScreenManager.h"
#include "ScreenSelectProfile.h"
#include "PluginManager.h"
#include "LuaManager.h"
#include "RageLog.h"
#include "archutils/Win32/ErrorStrings.h"

using namespace std;

bool ProfileState::GetPlayer(string playerId, Json::Value& data, string identifier)
{
	Json::Value response;
	string fields = "{ id nickname avatarIconUrl }";

	if (identifier == "nickname")
	{
		if (!PADMISS_CLIENT.SendGraphql("{ Players(where:{nickname:{eq:\"" + playerId + "\"}}) { nodes " + fields + " } }", response))
			return false;
	}
	else if (identifier == "rfid")
	{
		if (!PADMISS_CLIENT.SendGraphql("{ Players(where: { rfidCards: { some: { key: {eq: \"" + playerId + "\"} active: {eq:true} } } } ) { nodes " + fields + " } }", response))
			return false;
	}
	else if (identifier == "id")
	{
		if (!PADMISS_CLIENT.SendGraphql("{ Players(where:{id:{eq:" + playerId + "}}) { nodes " + fields + " } }", response))
			return false;
	}
	else
	{
		return false;
	}

	if (!response.isMember("Players") || response["Players"]["nodes"].size() == 0)
		return false;

	int index = 0;
	data = response["Players"]["nodes"][index];
	return true;
}

bool ProfileState::InstallProfile(Json::Value player, RString& profileId)
{
	bool match = false;

	vector<RString>* profileIds = APP_PTR(vector<RString>);
	PROFILEMAN->GetLocalProfileIDs(*profileIds);

	for (auto& p : *profileIds)
		if (PadmissId(p) == player["id"].asString())
		{
			match = true;
			break;
		}

	PLUGINMAN->AppDelete(profileIds);

	if (match)
		return false;

	if (!PROFILEMAN->CreateLocalProfile(player["nickname"].asString(), profileId))
		return false;

	Profile* profile = PROFILEMAN->GetLocalProfile(profileId);

	if (!profile)
		return false;

	SetPadmissId(profile, player["id"].asString());

	PROFILEMAN->SaveLocalProfile(profileId);

	return true;
}

RString ProfileState::PadmissId(RString profileId)
{
	Profile* profile = PROFILEMAN->GetLocalProfile(profileId);
	return PadmissId(profile);
}
RString ProfileState::PadmissId(Profile* profile)
{
	RString ret = "";
	
	if (!profile)
		return ret;

	Lua* L = LUA->Get();

	if (!profile->m_UserTable.IsSet())
		return ret;

	profile->m_UserTable.Get(L, "PadmissId");
	LuaHelpers::FromStack(L, ret, 0);
	lua_pop(L, 1);

	LUA->Release(L);

	return ret;
}

void ProfileState::SetPadmissId(Profile* profile, string padmissId)
{
	Lua* L = LUA->Get();

	lua_newtable(L);
	lua_pushstring(L, "PadmissId");
	lua_pushstring(L, padmissId.c_str());
	lua_settable(L, -3);
	profile->m_UserTable.SetFromStack(L);
	lua_settop(L, 0);

	LUA->Release(L);
}


int ProfileState::StartThread(void* p)
{
	((ProfileState*)p)->ThreadFunc();
	return 0;
}

void ProfileState::SetupThread()
{
	if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST))
		LOG->Warn(werr_ssprintf(GetLastError(), "Failed to set ProfileState thread priority"));
}

void ProfileState::ThreadFunc()
{
	unique_ptr<PMReader> reader;
	if (playerNumber == 0)
	{
		reader = make_unique<PMReader>();
	}

	while (!stopRequested)
	{
		if (reader)
		{
			queuedId = reader->Read();
		}
	}
}

ProfileState::ProfileState(int playerNumber)
{
	this->playerNumber = playerNumber;
}

void ProfileState::Init()
{
	m_thread.SetName("ProfileState");
	m_thread.Create(StartThread, this);
}

ProfileState::~ProfileState()
{
	if (m_thread.IsCreated())
	{
		LOG->Trace("PadmissPlugin :: Shutting down ProfileState thread ...");
		stopRequested = true;
		m_thread.Wait();
		LOG->Trace("PadmissPlugin :: ProfileState thread shut down.");
	}
}

bool ProfileState::ScanningAllowed()
{
	if (SCREENMAN->GetTopScreen())
	{
		screenName = SCREENMAN->GetTopScreen()->GetName();
	}
	
	if (screenName == "ScreenGameplay" || screenName == "ScreenEvaluation")
		return false;

	return true;
}

void ProfileState::Update()
{
	if (!ScanningAllowed())
		return;

	if (queuedId.length() > 0)
	{
		SignIn(queuedId, "rfid");
		queuedId = "";
	}
}

bool ProfileState::SignIn(string playerId, string identifier)
{
	if (!ScanningAllowed())
		return false;

	RString screenName = SCREENMAN->GetTopScreen()->GetName();

	Json::Value data;
	if (!GetPlayer(playerId, data, identifier))
		return false;

	vector<RString>* profileIds = APP_PTR(vector<RString>);
	PROFILEMAN->GetLocalProfileIDs(*profileIds);

	int nprofiles = PROFILEMAN->GetNumLocalProfiles();
	RString smProfileId = "";
	int profileIndex = -1;

	for (int i=0; i<profileIds->size(); i++)
	{
		RString id = profileIds->at(i);
		if (PadmissId(id).length() > 0)
		{
			smProfileId = id;
			profileIndex = i;
			break;
		}
	}

	if (smProfileId.length() == 0)
	{
		if (!InstallProfile(data, smProfileId))
			return false;

		if (screenName == "ScreenSelectProfile")
		{
			//SCREENMAN->GetTopScreen()->BeginScreen();
		}
	}

	PlayerNumber pn = PlayerNumber(playerNumber);

	if(!GAMESTATE->IsPlayerEnabled(pn))
	{
		GAMESTATE->JoinPlayer(pn);
	}

	PROFILEMAN->m_sDefaultLocalProfileID[pn].Set(smProfileId);
	PROFILEMAN->LoadLocalProfileFromMachine(pn);
	GAMESTATE->LoadCurrentSettingsFromProfile(pn);
	
	SCREENMAN->SetNewScreen("ScreenProfileLoad");


	/*
	if (screenName == "ScreenSelectProfile")
	{
		ScreenSelectProfile* currentScreen = (ScreenSelectProfile*)SCREENMAN->GetTopScreen();
		currentScreen->SetProfileIndex(pn, profileIndex);
	}
	*/


	PLUGINMAN->AppDelete(profileIds);

	return true;
}

void ProfileState::SignOut()
{
	PlayerNumber pn = PlayerNumber(playerNumber);

	if (GAMESTATE->IsPlayerEnabled(pn))
	{
		PROFILEMAN->SaveProfile(pn);
		GAMESTATE->UnjoinPlayer(pn);

		if (GAMESTATE->GetNumPlayersEnabled() == 0)
		{
			SCREENMAN->SetNewScreen("ScreenGameOver");
		}
		else
		{
			SCREENMAN->SetNewScreen("ScreenProfileSave");
		}
		
	}

}

#pragma once

#include "Reader.h"
#include "RageThreads.h"
#include "JsonUtil.h"
#include "Profile.h"

class ProfileState
{
public:
	ProfileState(int playerNumber);
	~ProfileState();

	void Update();
	bool SignIn(std::string playerId, std::string identifier = "nickname");
	void SignOut();

	void Init();
	static int StartThread(void* p);
	void ThreadFunc();
	bool ScanningAllowed();

	static bool GetPlayer(std::string playerId, Json::Value& data, std::string identifier = "nickname");
	static bool InstallProfile(Json::Value data, RString& profileId);
	static void SetPadmissId(Profile* profile, string padmissId);

	static RString PadmissId(RString profileId);
	static RString PadmissId(Profile* profile);

protected:
	int playerNumber;
	std::string queuedId = "";
	RString screenName;

	RageThread m_thread;
	bool stopRequested = false;
};

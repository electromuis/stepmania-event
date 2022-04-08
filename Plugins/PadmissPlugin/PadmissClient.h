#pragma once

#include "config.hpp"
#include "global.h"
#include "PluginManager.h"
#include "PlayerNumber.h"
#include "NoteData.h"
#include "Steps.h"
#include "StageStats.h"

#include <curl/curl.h>
#include <mutex>

class PadmissPlugin;

namespace Json {
	class Value;
};

class PadmissClient {
public:
	PadmissClient();
	~PadmissClient();

	void Initialize(std::string baseUrl);

	bool SendPost(std::string path, Json::Value& requestData, Json::Value* responseData = nullptr);
	bool SendGraphql(std::string request, Json::Value& out);

	bool SendScoreV1(Json::Value& score);
	bool SendScoreV2(Json::Value& score);
	std::string ChartHash(Steps* steps, Song* song);
	bool IsWorking();
	bool Login(std::string username, std::string password);
	bool IsLoggedIn() { return playerId.length() > 0; };
	std::string GetPlayerId() { return playerId; };
	std::string GetPlayerName() { return playerName; };

	std::mutex mtx;

private:
	bool Login(bool refresh = false);

	bool initialized = false;
	std::string baseUrl;
	std::string userId = "";
	std::string playerId = "";
	std::string playerName = "";
};

extern PadmissClient PADMISS_CLIENT;

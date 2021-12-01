#pragma once

#include "config.hpp"
#include "global.h"
#include "PluginManager.h"
#include "PlayerNumber.h"
#include "NoteData.h"
#include "Steps.h"
#include "StageStats.h"

#include <curl/curl.h>
#include "json/value.h"

class PadmissPlugin;

class PadmissClient {
public:
	PadmissClient(std::string baseUrl, std::string token);
	~PadmissClient();

	bool SendPost(std::string path, Json::Value& score);
	bool SendGraphql(std::string request, Json::Value& out);

	bool SendScoreV1(Json::Value& score);
	bool SendScoreV2(Json::Value& score);
	std::string ChartHash(Steps* steps, Song* song);
	bool IsWorking();
	bool Login(std::string username, std::string password);
	bool IsLoggedIn() { return playerId.length() > 0; }
	std::string GetPlayerId() { return playerId; }

private:
	CURL* curl = nullptr;
	std::string baseUrl;
	std::string token;
	std::string userToken;
	Json::Value* lastResponse = nullptr;
	bool working = false;
	std::string userId = "";
	std::string playerId = "";
};

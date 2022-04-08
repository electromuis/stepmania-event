#pragma once

#include "config.hpp"
#include "global.h"
#include "PluginManager.h"
#include "PlayerNumber.h"
#include "NoteData.h"
#include "Steps.h"
#include "StageStats.h"

#include <mutex>

class GrooveStatsPlugin;
namespace curlpp {
	class Easy;
}
namespace Json {
	class Value;
}

struct SharedRequestData
{
	string path;
	vector<pair<string, string>> query;
	vector<pair<string, string>> headers;

	void Apply(curlpp::Easy& req);
};

class GrooveStatsClient {
public:
	GrooveStatsClient();
	~GrooveStatsClient();

	void Initialize(std::string baseUrl);

	bool SendPost(SharedRequestData req, Json::Value& requestData, Json::Value* responseData = nullptr);
	bool SendGet(SharedRequestData req, Json::Value* responseData = nullptr);
	string GetBaseUrl() { return baseUrl; }

	bool HandleRequest(Json::Value& input, Json::Value& output);
	static const int GSProtocol;
private:
	bool initialized = false;
	std::string baseUrl;
};

extern GrooveStatsClient GROOVESTATS_CLIENT;

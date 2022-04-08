#pragma once

#define READ_REQUIRED_FIELD(field, type) \
	if (!input.isMember(#field) || !input[#field]. is ## type ()) \
		return false; \
	this->field = input[#field]. as ## type ();

#define READ_FIELD(field, type) \
	if (input.isMember(#field) && input[#field]. is ## type ()) \
		this->field = input[#field]. as ## type ();

#define READ_REQUIRED_OBJECT(field, type) \
	if (!input.isMember(#field)) \
		return false; \
	this->field = std::make_unique<type>(input[#field]); \
	if(!this->field->Validate()) \
		return false;

#define READ_OBJECT(field, type) \
	if (input.isMember(#field)) { \
		this->field = std::make_unique<type>(input[#field]); \
		if (!this->field->Validate()) \
			return false; \
	}
	

class ApiField
{
public:
	ApiField(Json::Value& json)
	{
		this->input = json;
	}
	
	virtual bool Validate() = 0;

protected:
	Json::Value input;
};

class GSRequest : public ApiField
{
public:
	using ApiField::ApiField;

	virtual bool Validate()
	{
		READ_FIELD(Id, String);
		return true;
	}

	virtual bool GetResponse(Json::Value& output) = 0;
protected:
	string Id;
};

class PingRequest : public GSRequest
{
public:
	using GSRequest::GSRequest;

	bool Validate()
	{
		if (!GSRequest::Validate())
			return false;
		
		READ_REQUIRED_FIELD(protocol, Int);

		if (GrooveStatsClient::GSProtocol != protocol)
			return false;

		return true;
	}

	bool GetResponse(Json::Value& output)
	{
		output["Major"] = "0.1";
		output["Minor"] = "0.1";
		output["Patch"] = "0.1";

		return true;
	}

protected:
	int protocol;
};

class NewSessionRequest : public GSRequest
{
public:
	using GSRequest::GSRequest;

	bool Validate()
	{
		if (!GSRequest::Validate())
			return false;

		READ_REQUIRED_FIELD(ChartHashVersion, Int);

		return true;
	}

	bool GetResponse(Json::Value& output)
	{
		vector<pair<string, string>> params;
		params.push_back(pair<string, string>(string("chartHashVersion"), to_string(ChartHashVersion)));

		if (!GROOVESTATS_CLIENT.SendGet({ "/new-session.php", params }, &output))
			return false;

		if (!output.isMember("servicesAllowed"))
			return false;

		return true;
	}

protected:
	int ChartHashVersion;
};

class PlayerData : public ApiField
{
public:
	using ApiField::ApiField;

	virtual bool Validate()
	{
		READ_REQUIRED_FIELD(chartHash, String);
		READ_REQUIRED_FIELD(apiKey, String);

		return true;
	}

	string GetChartHash() { return chartHash; };
	string GetApiKey() { return apiKey; };

protected:
	string chartHash;
	string apiKey;
};

class PlayerScoreRequest : public GSRequest
{
public:
	using GSRequest::GSRequest;

	bool Validate()
	{
		if (!GSRequest::Validate())
			return false;

		READ_OBJECT(player1, PlayerData);
		READ_OBJECT(player2, PlayerData);

		return true;
	}

	bool GetResponse(Json::Value& output)
	{
		vector<pair<string, string>> headers;
		vector<pair<string, string>> params;

		if (player1)
		{
			headers.push_back(pair<string, string>("x-api-key-player-1", player1->GetApiKey()));
			params.push_back(pair<string, string>("chartHashP1", player1->GetChartHash()));
		}

		if (player2)
		{
			headers.push_back(pair<string, string>("x-api-key-player-2", player2->GetApiKey()));
			params.push_back(pair<string, string>("chartHashP2", player2->GetChartHash()));
		}
			

		if (!GROOVESTATS_CLIENT.SendGet({"/player-scores.php", params, headers}, &output))
			return false;

		return true;
	}

protected:
	std::unique_ptr<PlayerData> player1;
	std::unique_ptr<PlayerData> player2;
};

class GsPlayerLeaderboardsRequest : public PlayerScoreRequest
{
public:
	using PlayerScoreRequest::PlayerScoreRequest;

	bool Validate()
	{
		if (!PlayerScoreRequest::Validate())
			return false;

		READ_FIELD(maxLeaderboardResults, Int);

		return true;
	}

	bool GetResponse(Json::Value& output)
	{
		vector<pair<string, string>> headers;
		vector<pair<string, string>> params;

		if (player1)
		{
			headers.push_back(pair<string, string>("x-api-key-player-1", player1->GetApiKey()));
			params.push_back(pair<string, string>("chartHashP1", player1->GetChartHash()));
		}

		if (player2)
		{
			headers.push_back(pair<string, string>("x-api-key-player-2", player2->GetApiKey()));
			params.push_back(pair<string, string>("chartHashP2", player2->GetChartHash()));
		}

		if (maxLeaderboardResults > 0)
			params.push_back(pair<string, string>("maxLeaderboardResults", to_string(maxLeaderboardResults)));

		if (!GROOVESTATS_CLIENT.SendGet({ "/player-leaderboards.php", params, headers }, &output))
			return false;

		return true;
	}

protected:
	int maxLeaderboardResults = -1;
};

class Judgements : public ApiField
{
public:
	Judgements(Json::Value& input)
		:ApiField(input)
	{
		fields.push_back("fantasticPlus");
		fields.push_back("fantastic");
		fields.push_back("excellent");
		fields.push_back("great");
		fields.push_back("decent");
		fields.push_back("wayOff");
		fields.push_back("miss");
		fields.push_back("totalSteps");
		fields.push_back("minesHit");
		fields.push_back("totalMines");
		fields.push_back("holdsHeld");
		fields.push_back("totalHolds");
		fields.push_back("rollsHeld");
		fields.push_back("totalRolls");
	}
	
	virtual bool Validate()
	{
		for (auto& f : fields)
		{
			if (!input.isMember(f))
				return false;

			if (!input[f].isInt())
				return false;

			if (input[f].asInt() < 0)
				return false;

			values.emplace(f, input[f].asInt());
		}

		return true;
	}

	void Apply(Json::Value& json)
	{
		for (auto& f : fields)
		{
			if (values.count(f) == 0)
				continue;

			json[f] = values.at(f);
		}
	}


protected:
	vector<string> fields;
	map<string, int> values;
};

class ScoreSubmitPlayerData : public ApiField
{
public:
	using ApiField::ApiField;

	virtual bool Validate()
	{
		READ_REQUIRED_FIELD(apiKey, String);
		READ_FIELD(profileName, String);
		READ_REQUIRED_FIELD(chartHash, String);

		READ_REQUIRED_FIELD(score, Int);
		if (score < 0 || score > 10000)
			return false;

		READ_FIELD(comment, String);
		READ_FIELD(rate, Int);
		READ_FIELD(usedCmod, Bool);

		READ_OBJECT(judgmentCounts, Judgements);

		return true;
	}

	void Apply(Json::Value& json)
	{
		json["score"] = score;
		if(comment.length() > 0)
			json["comment"] = comment;
		json["rate"] = rate;
		json["usedCmod"] = usedCmod;

		if (judgmentCounts)
			judgmentCounts->Apply(json["judgmentCounts"]);
	}

	string GetChartHash() { return chartHash; };
	string GetApiKey() { return apiKey; };

protected:
	string apiKey = "";
	string profileName = "";
	string chartHash = "";
	int score = 0;
	string comment = "";
	int rate = 1;
	bool usedCmod = true;
	std::unique_ptr<Judgements> judgmentCounts;
};

class ScoreSubmitRequest : public GSRequest
{
public:
	using GSRequest::GSRequest;

	bool Validate()
	{
		if (!GSRequest::Validate())
			return false;

		READ_FIELD(maxLeaderboardResults, Int);
		READ_OBJECT(player1, ScoreSubmitPlayerData);
		READ_OBJECT(player2, ScoreSubmitPlayerData);

		return true;
	}

	bool GetResponse(Json::Value& output)
	{
		vector<pair<string, string>> headers;
		vector<pair<string, string>> params;

		Json::Value json;

		if (player1)
		{
			headers.push_back(pair<string, string>("x-api-key-player-1", player1->GetApiKey()));
			params.push_back(pair<string, string>("chartHashP1", player1->GetChartHash()));

			player1->Apply(json["player1"]);
		}

		if (player2)
		{
			headers.push_back(pair<string, string>("x-api-key-player-2", player2->GetApiKey()));
			params.push_back(pair<string, string>("chartHashP2", player2->GetChartHash()));

			player2->Apply(json["player2"]);
		}

		if (maxLeaderboardResults > 0)
			params.push_back(pair<string, string>("maxLeaderboardResults", to_string(maxLeaderboardResults)));


		if (!GROOVESTATS_CLIENT.SendPost({ "/score-submit.php", params, headers }, json, &output))
			return false;

		return true;
	}

protected:
	int maxLeaderboardResults = -1;
	std::unique_ptr<ScoreSubmitPlayerData> player1;
	std::unique_ptr<ScoreSubmitPlayerData> player2;
};

#pragma once

#include "GrooveStatsClient.h"

#include <stdio.h>
#include "global.h"

#include "json/json.h"

#include "curlpp/cURLpp.hpp"
#include "curlpp/Easy.hpp"
#include <curlpp/Options.hpp>
#include <curlpp/Infos.hpp>

#include "GSReguests.h"

using namespace curlpp::options;
using namespace std;


GrooveStatsClient GROOVESTATS_CLIENT;
const int GrooveStatsClient::GSProtocol = 1;

void SharedRequestData::Apply(curlpp::Easy& req)
{
	string fullPath = path;

	if (query.size() > 0)
		fullPath.append("?");

	for (int q = 0; q < query.size(); q++)
	{
		if (q > 0)
			fullPath.append("&");

		fullPath.append(query.at(q).first);
		fullPath.append("=");
		fullPath.append(query.at(q).second);
	}

	list<std::string> fullHeaders;
	fullHeaders.push_back("Content-Type: application/json");

	for (int q = 0; q < headers.size(); q++)
	{
		string headerRow = headers.at(q).first;
		headerRow.append(":");
		headerRow.append(headers.at(q).second);
		fullHeaders.push_back(headerRow);
	}

	req.setOpt<Url>(GROOVESTATS_CLIENT.GetBaseUrl() + fullPath);
	req.setOpt(new HttpHeader(fullHeaders));
}

GrooveStatsClient::GrooveStatsClient()
{
	
}

GrooveStatsClient::~GrooveStatsClient()
{
}

void GrooveStatsClient::Initialize(string baseUrl)
{
	this->baseUrl = baseUrl;
	this->initialized = true;
}

bool GrooveStatsClient::SendPost(SharedRequestData req, Json::Value& requestData, Json::Value* responseData)
{
	curlpp::Cleanup myCleanup;
	curlpp::Easy myRequest;

	req.Apply(myRequest);

	Json::FastWriter writer;
	string request = writer.write(requestData);
	
	myRequest.setOpt(new PostFields(request));
	myRequest.setOpt(new PostFieldSize(request.length()));

	ostringstream responseStream;
	myRequest.setOpt(new curlpp::options::WriteStream(&responseStream));

	myRequest.perform();
	int http_code = curlpp::infos::ResponseCode::get(myRequest);
	string responseString = responseStream.str();

	Json::Reader reader;

	

	LOG->Info("GrooveStatsClient - Path: %s - Request: %s - Response: %s", req.path.c_str(), request.c_str(), responseString.c_str());

	if (responseData)
	{
		if (!reader.parse(responseString, *responseData)) {
			LOG->Info("GrooveStatsClient: %s invalid: %s", req.path.c_str(), responseString.c_str());
			return false;
		}
		else
			return true;

	}

	return true;
}

bool GrooveStatsClient::SendGet(SharedRequestData req, Json::Value* responseData)
{
	curlpp::Cleanup myCleanup;
	curlpp::Easy myRequest;

	req.Apply(myRequest);

	ostringstream responseStream;
	myRequest.setOpt(new curlpp::options::WriteStream(&responseStream));

	myRequest.perform();
	int http_code = curlpp::infos::ResponseCode::get(myRequest);
	string responseString = responseStream.str();

	Json::Reader reader;

	LOG->Info("GrooveStatsClient - Path: %s - Response: %s", req.path.c_str(), responseString.c_str());

	if (responseData)
	{
		if (!reader.parse(responseString, *responseData)) {
			LOG->Info("GrooveStatsClient: %s invalid: %s", req.path.c_str(), responseString.c_str());
			return false;
		}
		else
			return true;

	}

	return true;
}

bool GrooveStatsClient::HandleRequest(Json::Value& input, Json::Value& output)
{
	if (!input.isMember("action") || !input["action"].isString())
		return false;

	unique_ptr<GSRequest> request;
	string action = input["action"].asString();

	if (action == "ping")
		request = make_unique<PingRequest>(input);
	else if (action == "groovestats/new-session")
		request = make_unique<NewSessionRequest>(input);
	else if (action == "groovestats/player-scores")
		request = make_unique<PlayerScoreRequest>(input);
	else if (action == "groovestats/player-leaderboards")
		request = make_unique<GsPlayerLeaderboardsRequest>(input);
	else if (action == "groovestats/score-submit")
		request = make_unique<ScoreSubmitRequest>(input);
	else
	{
		return false;
	}

	if (!request->Validate())
		return false;

	return request->GetResponse(output);
}

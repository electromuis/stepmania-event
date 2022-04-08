#pragma once

#include <stdio.h>
#include "global.h"
#include <tomcrypt.h>

#include "json/json.h"

#include "PadmissClient.h"
#include "NotesWriterPadmiss.h"

#include "curlpp/cURLpp.hpp"
#include "curlpp/Easy.hpp"
#include <curlpp/Options.hpp>
#include <curlpp/Infos.hpp>

using namespace curlpp::options;
using namespace std;

PadmissClient PADMISS_CLIENT;
static Preference<RString> g_padmissCabToken("PadmissCabToken", "");
static Preference<RString> g_padmissUserToken("PadmissUserToken", "");
static Preference<RString> g_padmissUserTokenRefresh("PadmissUserTokenRefresh", "");

PadmissClient::PadmissClient()
{
	
}

PadmissClient::~PadmissClient()
{
}

void PadmissClient::Initialize(string baseUrl)
{
	this->baseUrl = baseUrl;
	if (g_padmissUserToken.Get().length() > 0)
	{
		Login(true);
	}
	this->initialized = true;
}

struct responseString {
	char* ptr;
	size_t len;
};

void InitCurlResponseString(struct responseString* s) {
	s->len = 0;
	s->ptr = (char*)malloc(s->len + 1);
	if (s->ptr == NULL) {
		fprintf(stderr, "malloc() failed\n");
		exit(EXIT_FAILURE);
	}
	s->ptr[0] = '\0';
}

size_t ReadCurlResponse(void* ptr, size_t size, size_t nmemb, struct responseString* s)
{
	size_t num_read = size * nmemb;
	size_t new_len = s->len + num_read;
	s->ptr = (char*)realloc(s->ptr, new_len + 1);
	if (s->ptr == NULL) {
		fprintf(stderr, "realloc() failed\n");
		exit(EXIT_FAILURE);
	}
	memcpy(s->ptr + s->len, ptr, size * nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;

	return num_read;
}

bool PadmissClient::SendPost(std::string path, Json::Value& requestData, Json::Value* responseData)
{
	Json::FastWriter writer;
	string request = writer.write(requestData);

	curlpp::Cleanup myCleanup;
	curlpp::Easy myRequest;

	myRequest.setOpt<Url>(baseUrl + path);
	myRequest.setOpt(new PostFields(request));
	myRequest.setOpt(new PostFieldSize(request.length()));

	list<std::string> headers;
	headers.push_back("Content-Type: application/json");
	if (IsLoggedIn())
	{
		char token[1024] = "";
		g_padmissUserToken.Get().copy(token, 1024);
		
		string tokenString = token;
		
		headers.push_back("Authorization: Bearer " + tokenString);
	}
	myRequest.setOpt(new HttpHeader(headers));

	ostringstream responseStream;
	myRequest.setOpt(new curlpp::options::WriteStream(&responseStream));

	myRequest.perform();
	int http_code = curlpp::infos::ResponseCode::get(myRequest);
	string responseString = responseStream.str();

	Json::Reader reader;

	LOG->Info("Padmiss api - Path: %s - Request: %s - Response: %s", path.c_str(), request.c_str(), responseString.c_str());

	if (responseData)
	{
		if (!reader.parse(responseString, *responseData)) {
			LOG->Info("Padmiss api: %s invalid: %s", path.c_str(), responseString.c_str());
			return false;
		}
		else
			return true;

	}

	return true;
}

bool PadmissClient::SendGraphql(std::string request, Json::Value& out)
{
	Json::Value jsonRequest;
	Json::Value response;
	jsonRequest["query"] = request;

	if (!SendPost("/v2/graphql", jsonRequest, &response)) {
		return false;
	}

	if (!response["data"].isObject()) {
		return false;
	}

	out = response["data"];
	return true;
}

bool PadmissClient::SendScoreV1(Json::Value& score)
{
	score["apiKey"] = g_padmissCabToken.Get();

	return SendPost("/post-score", score);
}

bool PadmissClient::SendScoreV2(Json::Value& score)
{
	return SendPost("/v2/v1/scores?ApiKey=" + g_padmissCabToken.Get(), score);
}

string PadmissClient::ChartHash(Steps* steps, Song* song)
{
	Json::Value* chart = APP_PTR(Json::Value);
	Json::Value response;

	NotesWriterPadmiss::WriteSong(*chart, *song);
	NotesWriterPadmiss::WriteSteps(*chart, *steps);

	string cabToken = g_padmissCabToken.Get().c_str();

	if (!SendPost("/v2/v1/stepCharts?ApiKey=" + cabToken, *chart, &response)) {
		PLUGINMAN->AppDelete(chart);
		return "";
	}

	if (response["stepChartHash"].isString()) {
		PLUGINMAN->AppDelete(chart);
		return response["stepChartHash"].asCString();
	}

	PLUGINMAN->AppDelete(chart);

	return "";
}

bool PadmissClient::Login(std::string username, std::string password)
{
	Json::Value request;
	Json::Value response;
	request["email"] = username;
	request["password"] = password;

	if (!SendPost("/v2/v1/authenticate", request, &response)) {
		return false;
	}

	if (!response["token"].isString()) {
		return false;
	}
	
	g_padmissUserToken.Set(response["token"].asString());
	g_padmissUserTokenRefresh.Set(response["refreshToken"].asString());

	return Login();
}

bool PadmissClient::Login(bool refresh)
{
	char token[1024] = "";
	g_padmissUserToken.Get().copy(token, 1024);
	string userToken = token;
	if (userToken.length() == 0)
		return false;

	if (refresh)
	{
		char tokenRefresh[1024] = "";
		g_padmissUserTokenRefresh.Get().copy(tokenRefresh, 1024);
		string tokenRefreshString = tokenRefresh;

		if (tokenRefreshString.length() == 0)
			return false;

		Json::Value request;
		Json::Value response;
		request["token"] = userToken;
		request["refreshToken"] = tokenRefreshString;

		if (!SendPost("/v2/v1/refresh-token", request, &response)) {
			return false;
		}

		if (!response["token"].isString()) {
			return false;
		}

		userToken = response["token"].asString();

		g_padmissUserToken.Set(response["token"].asString());
		g_padmissUserTokenRefresh.Set(response["refreshToken"].asString());
	}
	

	// Todo: a JWT library without headaces
	auto end = userToken.find(".");
	size_t pos = 0;
	int part = 0;

	string newPlayerId;
	string newUserId;

	while (end != string::npos) {
		if (part == 1) {
			string tokenPart = userToken.substr(pos, end - pos);
			unsigned char buffer[255];
			unsigned long bufferSize = 255;

			base64url_decode((const unsigned char*)tokenPart.c_str(), tokenPart.length(), buffer, &bufferSize);

			if (strlen((const char*)buffer) == 0) {
				return false;
			}

			Json::Reader reader;
			Json::Value response;

			if (!reader.parse((const char*)buffer, response)) {
				return false;
			}

			Json::FastWriter writer;
			RString parsed = writer.write(response);

			if (!response["playerId"].isString() || !response["userId"].isString()) {
				return false;
			}

			newPlayerId = response["playerId"].asString();
			newUserId = response["userId"].asString();

			break;
		}

		pos = end + 1;
		end = userToken.find(".", pos);
		part++;
	}

	if (newPlayerId.length() == 0)
		return false;
	
	Json::Value response;
	
	if (!SendGraphql("{Player(id:" + newPlayerId + ") {nickname}}", response))
		return false;

	if (!response.isMember("Player") || response["Player"].isNull())
		return false;

	playerName = response["Player"]["nickname"].asString();

	playerId = newPlayerId;
	userId = newUserId;

	return true;
}

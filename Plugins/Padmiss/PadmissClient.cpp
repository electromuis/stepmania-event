#pragma once

#include <stdio.h>
#include "global.h"
#include <tomcrypt.h>

#include "json/writer.h"
#include "json/reader.h"

#include "PadmissClient.h"
#include "NotesWriterPadmiss.h"


PadmissClient::PadmissClient(std::string baseUrl, std::string token)
	:baseUrl(baseUrl),
	token(token)
{
	
}

PadmissClient::~PadmissClient()
{
	if (lastResponse != nullptr) {
		delete lastResponse;
	}
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

bool PadmissClient::SendPost(std::string path, Json::Value& score)
{
	ASSERT_M(!working, "Only one request at a time please");

	working = true;
	bool success = false;
	if (lastResponse != nullptr) {
		delete lastResponse;
	}

	Json::FastWriter writer;
	std::string request = writer.write(score);
	
	CURLcode res;
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	if (curl) {
		struct responseString bodyString;
		InitCurlResponseString(&bodyString);
		struct responseString headerString;
		InitCurlResponseString(&headerString);
		char errbuf[CURL_ERROR_SIZE];
		errbuf[0] = 0;

		struct curl_slist* chunk = NULL;
		chunk = curl_slist_append(chunk, "Content-Type: application/json");

		std::string url = baseUrl + path;

		LOG->Info("Padmiss api: %s - %s", url.c_str(), request.c_str());

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, request.length());
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, false);

		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &ReadCurlResponse);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &ReadCurlResponse);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &bodyString);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headerString);

		res = curl_easy_perform(curl);

		long http_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		
		if (res != CURLE_OK) {
			LOG->Info("Padmiss api: %s failed: %s", url.c_str(), curl_easy_strerror(res));

			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

			size_t len = strlen(errbuf);
			fprintf(stderr, "\nlibcurl: (%d) ", res);
			if (len)
				fprintf(stderr, "%s%s", errbuf,
					((errbuf[len - 1] != '\n') ? "\n" : ""));
			else
				fprintf(stderr, "%s\n", curl_easy_strerror(res));
		}
		else {
			Json::Reader reader;
			Json::Value response;
			
			lastResponse = new Json::Value();
			if (!reader.parse(bodyString.ptr, *lastResponse)) {
				LOG->Info("Padmiss api: %s invalid: %s", url.c_str(), bodyString.ptr);

				delete lastResponse;
			}
			else {
				LOG->Info("Padmiss api: %s response: %s", url.c_str(), bodyString.ptr);

				success = true;
			}
		}

		curl_easy_cleanup(curl);
		free(headerString.ptr);
		free(bodyString.ptr);
	}

	curl_global_cleanup();

	working = false;
	return success;
}

bool PadmissClient::SendGraphql(std::string request, Json::Value& out)
{
	Json::Value jsonRequest;
	jsonRequest["query"] = request;

	if (!SendPost("/v2/graphql", jsonRequest)) {
		return false;
	}

	if (!(*lastResponse)["data"].isObject()) {
		return false;
	}

	out = (*lastResponse)["data"];
	return true;
}

bool PadmissClient::SendScoreV1(Json::Value& score)
{
	score["apiKey"] = token;

	return SendPost("/post-score", score);
}

bool PadmissClient::SendScoreV2(Json::Value& score)
{
	return SendPost("/v2/v1/scores?ApiKey=" + token, score);
}

string PadmissClient::ChartHash(Steps* steps, Song* song)
{
	Json::Value chart;

	NotesWriterPadmiss::WriteSong(chart, *song);
	NotesWriterPadmiss::WriteSteps(chart, *steps);

	if (!SendPost("/v2/v1/stepCharts?ApiKey=" + token, chart)) {
		return "";
	}

	if (lastResponse && (*lastResponse)["stepChartHash"].isString()) {
		return (*lastResponse)["stepChartHash"].asCString();
	}

	return "";
}

bool PadmissClient::Login(std::string username, std::string password)
{
	Json::Value request;
	request["email"] = username;
	request["password"] = password;

	if (!SendPost("/v2/v1/authenticate", request)) {
		return false;
	}

	if (!(*lastResponse)["token"].isString()) {
		return false;	
	}

	userToken = (*lastResponse)["token"].asCString();


	// Todo: a JWT library without headaces
	auto end = userToken.find(".");
	size_t pos = 0;
	int part = 0;

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

			playerId = response["playerId"].asString();
			userId = response["userId"].asString();

			break;
		}

		pos = end + 1;
		end = userToken.find(".", pos);
		part++;
	}

	return true;
}

bool PadmissClient::IsWorking()
{
	return working;
}

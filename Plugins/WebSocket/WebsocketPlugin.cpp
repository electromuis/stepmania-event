#pragma once

#include <iostream>
#include <fstream>
using namespace std;

#include "global.h"
#include "WebsocketPlugin.h"

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

#include "JsonUtil.h"
#include "json/writer.h"
#include "json/reader.h"
#include "RageLog.h"
#include "Song.h"
#include "SongManager.h"
#include "ScreenManager.h"
#include "ScreenManager.h"
#include "ScreenSelectMusic.h"
#include "LuaManager.h"
#include "RageUtil.h"
#include "archutils/Win32/ErrorStrings.h"

REGISTER_PLUGIN(WebSocketPlugin, PLUGIN_NAME, "0.0.1")

AutoScreenMessage(SM_SongChanged);

WebSocketHandler::WebSocketHandler(WebSocketPlugin* plugin)
	:m_plugin(plugin)
{
	ASSERT(!m_thread.IsCreated());

	docRoot = plugin->GetDocRoot();

	m_thread.SetName("WebSocketHandler");
	m_thread.Create(StartThread, this);
}

WebSocketHandler::~WebSocketHandler()
{
	if (m_thread.IsCreated())
	{
		LOG->Trace("WebSocket :: Shutting down WebSocketHandler thread ...");
		stopRequested = true;
		m_thread.Wait();
		LOG->Trace("WebSocket :: WebSocketHandler thread shut down.");
	}
}

void WebSocketHandler::ThreadFunc()
{
	m_server.set_open_handler(bind(&WebSocketHandler::on_open, this, ::_1));
	m_server.set_close_handler(bind(&WebSocketHandler::on_close, this, ::_1));
	m_server.set_message_handler(bind(&WebSocketHandler::on_message, this, ::_1, ::_2));

	if (docRoot) {
		m_server.set_http_handler(bind(&WebSocketHandler::on_http, this, _1));
		LOG->Trace("WebSocket :: Serving DocRoot: %s", docRoot);
	}
	else {
		LOG->Trace("WebSocket :: DocRoot not found: %s", docRoot);
	}

	m_server.init_asio();
	m_server.listen(8081);
	m_server.start_accept();

	LOG->Trace("WebSocket :: WebSocketHandler listening");

	while (!stopRequested)
	{
		m_server.run_one();
	}
}

void WebSocketHandler::on_open(connection_hdl hdl)
{
	
}

void WebSocketHandler::on_close(connection_hdl hdl)
{

}

void WebSocketHandler::on_http(connection_hdl hdl)
{
	ws_server::connection_ptr con = m_server.get_con_from_hdl(hdl);

	std::ifstream file;
	std::string filename = con->get_resource();
	std::string response;

	if (filename == "/") {
		filename = docRoot + "/index.html";
	}
	else {
		filename = docRoot + "/" + filename.substr(1);
	}

	file.open(filename.c_str(), std::ios::in);
	if (!file) {
		// 404 error
		std::stringstream ss;

		ss << "<!doctype html><html><head>"
			<< "<title>Error 404 (Resource not found)</title><body>"
			<< "<h1>Error 404</h1>"
			<< "<p>The requested URL " << filename << " was not found on this server.</p>"
			<< "</body></head></html>";

		con->set_body(ss.str());
		con->set_status(websocketpp::http::status_code::not_found);

		LOG->Warn(PLUGIN_NAME" :: File not found: %s", filename);

		return;
	}


	LOG->Warn(PLUGIN_NAME" :: Serving file: %s", filename);

	file.seekg(0, std::ios::end);
	response.reserve(file.tellg());
	file.seekg(0, std::ios::beg);

	response.assign((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());

	con->set_body(response);
	con->set_status(websocketpp::http::status_code::ok);
}

void WebSocketHandler::on_message(connection_hdl hdl, ws_server::message_ptr msg)
{
	Json::Value root;
	Json::Reader reader;

	if (!reader.parse(msg->get_payload(), root)) {
		RString err = reader.getFormatedErrorMessages();
		LOG->Warn(PLUGIN_NAME" :: Invalid json received %s", err.c_str());
		return;
	}

	if (!root["action"].isString())
	{
		LOG->Warn(PLUGIN_NAME" :: Message action missing");
		return;
	}

	Json::Value response;
	if (root["action"] == "GetGroups") {
		response["action"] = root["action"];

		GetGroups(response);
		response["success"] = true;
	}
	else if (root["action"] == "GetSongsInGroup") {
		response["action"] = root["action"];

		if (root["group"].isString()) {
			GetSongsInGroup(root["group"].asCString(), response);
			response["success"] = true;
		}
		else {
			response["success"] = false;
		}
	}
	else if (root["action"] == "SelectSong") {
		response["action"] = root["action"];

		if (root["group"].isString() && root["title"].isString()) {
			response["success"] = SelectSong(root["group"].asCString(), root["title"].asCString());
		}
		else {
			response["success"] = false;
		}
	}
	else {
		response["success"] = false;
		response["error"] = "Unknown action";
		LOG->Warn(PLUGIN_NAME" :: Invalid action received");
	}

	Json::FastWriter writer;
	msg->set_payload(writer.write(response));
	m_server.send(hdl, msg);
}

bool WebSocketHandler::SelectSong(RString group, RString songTitle)
{
	vector<Song*> songs = SONGMAN->GetSongs(group);
	Song* song = nullptr;
	for (Song* s: songs)
	{
		if (s->m_sMainTitle == songTitle) {
			song = s;
			break;
		}
	}

	if (song == nullptr)
	{
		return false;
	}

	Screen* curScreen = SCREENMAN->GetTopScreen();
	if (curScreen == nullptr)
	{
		return false;
	}

	if (ScreenSelectMusic* s = dynamic_cast<ScreenSelectMusic*>(curScreen)) {
		MusicWheel* wheel = s->GetMusicWheel();
		if (wheel != nullptr)
		{
			wheel->SelectSong(song);
			wheel->Select();
			wheel->Move(-1);
			wheel->Move(1);
			wheel->Select();
			return true;
		}
	}

	return false;
}

void WebSocketHandler::GetSongsInGroup(RString group, Json::Value& response)
{
	vector<Song*> songs = SONGMAN->GetSongs(group);
	for (Song* song : songs)
	{
		int s = response["songs"].size();
		response["songs"][s]["title"] = song->m_sMainTitle;
		response["songs"][s]["artist"] = song->m_sArtist;
		response["songs"][s]["banner"] = song->GetBannerPath();
	}
}

void WebSocketHandler::GetGroups(Json::Value& response)
{
	int numGroups = SONGMAN->GetNumSongGroups();
	for (int i = 0; i < numGroups; i++)
	{
		response["groups"][i] = SONGMAN->GetSongGroupByIndex(i);
	}
}

int WebSocketHandler::StartThread( void *p )
{
	((WebSocketHandler*)p)->ThreadFunc();
	return 0;
}

void WebSocketHandler::SetupThread()
{
	if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST))
		LOG->Warn(werr_ssprintf(GetLastError(), "Failed to set WebSocketHandler thread priority"));
}

WebSocketPlugin::WebSocketPlugin(std::string libraryPath)
	:subscriber(this)
	,libraryPath(libraryPath)
	,handler(this)
{	
	LOG->Info(PLUGIN_NAME" loaded");
}

WebSocketPlugin::~WebSocketPlugin()
{
	LOG->Info(PLUGIN_NAME" unloaded");
}

RString WebSocketPlugin::GetDocRoot()
{
	return "";
	/*
	RString lib;

	PLUGINMAN->CopyString(
		GetFileNameWithoutExtension(
			PLUGINMAN->MakeString(libraryPath.c_str())
		)
	);

	return "";

	RString folder, junk;
	splitpath(libraryPath, folder, junk, junk);

	if (lib && folder) {
		return folder + "/" + lib;
	}

	return "";
	*/
}

void WebSocketPlugin::Update(float fDeltaTime)
{

}

PluginMessageSubscriber::PluginMessageSubscriber(WebSocketPlugin* plugin)
	:plugin(plugin)
{

}

PluginMessageSubscriber::~PluginMessageSubscriber()
{
	UnsubscribeAll();
}

void PluginMessageSubscriber::HandleMessage(const Message& msg)
{
	
}

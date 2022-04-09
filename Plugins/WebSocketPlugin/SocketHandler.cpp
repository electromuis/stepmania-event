#include "SocketHandler.h"

#include "JsonUtil.h"
#include "RageLog.h"
#include "LuaManager.h"
#include "Song.h"
#include "SongManager.h"
#include "ScreenManager.h"
#include "ScreenSelectMusic.h"
#include "archutils/Win32/ErrorStrings.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include "winsock2.h"
#include <iostream>
#include <conio.h>
#endif

#include <chrono>
#include <fstream>
#include <thread>

#define UDP_KEY "SM5WS-"

#ifdef _WIN32
#define CLOSE_UDP() closesocket(this->udpSocket); this->udpSocket = -1;
#else
#define CLOSE_UDP() ::close(this->udpSocket); this->udpSocket = -1;
#endif

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

chrono::high_resolution_clock::time_point lastBroadcast;
std::map<RString, SocketFunction> functions;

WebSocketHandler::WebSocketHandler(WebSocketPlugin* plugin)
	:m_plugin(plugin)
{
	docRoot = plugin->GetDocRoot();

	auto me = this;

	RegisterFunction("SetWaiting", [me](SocketRequest* req) {
		if (!req->request.isMember("waiting"))
			return false;

		me->SetWaiting(req->request["waiting"].asBool());

		return true;
	});

	RegisterFunction("GetGroups", [me](SocketRequest* req) {
		me->GetGroups(req->response);
		return true;
		});

	RegisterFunction("GetSongsInGroup", [me](SocketRequest* req) {
		if (!req->request["group"].isString())
			return false;

		me->GetSongsInGroup(req->request["group"].asCString(), req->response);
		return true;
	});

	RegisterFunction("SelectSong", [me](SocketRequest* req) {
		if (!req->request["group"].isString())
			return false;

		if (!req->request["title"].isString())
			return false;

		return me->SelectSong(req->request["group"].asCString(), req->response["title"].asCString());
	});

	RegisterFunction("RunLua", [me](SocketRequest* req) {
		return me->RunLua(req->request, req->response);
	});

	RegisterFunction("Broadcast", [me](SocketRequest* req) {
		// Cab found

		return true;
	});

	localIp = "192.168.2.29";

	m_wsThread.SetName("WebSocketHandler WS");
	m_wsThread.Create(StartWSThread, this);

	m_udpThread.SetName("WebSocketHandler UDP");
	m_udpThread.Create(StartUDPThread, this);
}

WebSocketHandler::~WebSocketHandler()
{
	stopRequested = true;
	//LOG->Trace("WebSocket :: Shutting down WebSocketHandler threads ...");

	if (m_wsThread.IsCreated())
	{
		m_wsThread.Wait();
 		//LOG->Trace("WebSocket :: WebSocketHandler WS thread shut down.");
	}

	if (m_udpThread.IsCreated())
	{
		m_udpThread.Wait();
		//LOG->Trace("WebSocket :: WebSocketHandler UDP thread shut down.");
	}
}

// WS Thread

int WebSocketHandler::StartWSThread(void* p)
{
	((WebSocketHandler*)p)->WSThreadFunc();
	return 0;
}

void WebSocketHandler::WSThreadFunc()
{
	m_server.set_open_handler(bind(&WebSocketHandler::on_open, this, ::_1));
	m_server.set_close_handler(bind(&WebSocketHandler::on_close, this, ::_1));
	m_server.set_message_handler(bind(&WebSocketHandler::on_message, this, ::_1, ::_2));

	if (docRoot.length() > 0) {
		m_server.set_http_handler(bind(&WebSocketHandler::on_http, this, _1));
		LOG->Trace("WebSocket :: Serving DocRoot: %s", docRoot.c_str());
	}
	else {
		LOG->Trace("WebSocket :: DocRoot not found: %s", docRoot.c_str());
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

// UDP Thread

int WebSocketHandler::StartUDPThread(void* p)
{
	((WebSocketHandler*)p)->UDPThreadFunc();
	return 0;
}

void WebSocketHandler::UDPThreadFunc()
{
	while (!stopRequested)
	{
		ReadUDPBroadcast();

		auto t_now = std::chrono::high_resolution_clock::now();
		double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_now - lastBroadcast).count();
		if (elapsed_time_ms > 5000)
		{
			Json::Value message;
			message["action"] = "Broadcast";
			message["ip"] = localIp;
			message["port"] = 8001;

			SendUDPBroadcast(message);

			lastBroadcast = t_now;
		}
	}
}

struct sockaddr_in Recv_addr;

bool WebSocketHandler::SetupUDP()
{
	Recv_addr.sin_family = AF_INET;
	Recv_addr.sin_port = htons(8082);
	Recv_addr.sin_addr.s_addr = INADDR_ANY;

	if (this->udpSocket == -1)
	{
		this->udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
		if (this->udpSocket < 0)
		{
			CLOSE_UDP();
			return false;
		}

		char broadcast = '1';
		if (setsockopt(this->udpSocket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0)
		{
			CLOSE_UDP();
			return false;
		}

		int iTimeout = 1000;
		if (setsockopt(this->udpSocket,
			SOL_SOCKET,
			SO_RCVTIMEO,
			(const char*)&iTimeout,
			sizeof(iTimeout)) < 0)
		{
			CLOSE_UDP();
			return false;
		}

		if (::bind(this->udpSocket, (sockaddr*)&Recv_addr, sizeof(Recv_addr)) < 0)
		{
			CLOSE_UDP();
			return false;
		}
	}

	return true;
}

bool WebSocketHandler::ReadUDPBroadcast()
{
	if (!SetupUDP())
		return false;

	char buffer[2048] = "";
	socklen_t addrlen = sizeof Recv_addr;
	ssize_t received;
	struct sockaddr_in Sender_addr;

	if (recvfrom(this->udpSocket, buffer, 2048, 0, (struct sockaddr*)&Sender_addr, &addrlen) < 0)
		return false;

	struct sockaddr_in localAddr;
	inet_pton(AF_INET, localIp.c_str(), &(localAddr.sin_addr));

	return false;
	/*
	if (Sender_addr.sin_addr.S_un.S_addr == localAddr.sin_addr.S_un.S_addr)
	{
		// Local packet
		return false;
	}
	*/

	string data = buffer;
	if (data.substr(0, strlen(UDP_KEY)) != UDP_KEY)
		return false;
	data = data.substr(strlen(UDP_KEY));

	SocketRequest req;
	return req.HandleRequest(data);
}

bool WebSocketHandler::SendUDPBroadcast(Json::Value message)
{
	if (!SetupUDP())
		return false;

	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(8082);
	addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

	int len = sizeof(struct sockaddr_in);
	string udpMessage = UDP_KEY;

	Json::FastWriter writer;
	udpMessage.append(writer.write(message));

	const char* content = udpMessage.c_str();
	if (sendto(this->udpSocket, content, udpMessage.size(), 0, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		CLOSE_UDP();
		return false;
	}

	return true;
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

void WebSocketHandler::RegisterFunction(RString name, SocketFunction function)
{
	functions.emplace(name, function);
}

bool SocketRequest::HandleRequest(string requestString)
{
	try {
		Json::Reader reader;

		if (!reader.parse(requestString, request)) {
			string err = reader.getFormatedErrorMessages();
			throw std::runtime_error(err.c_str());
		}

		if (!request["action"].isString() || functions.count(request["action"].asString()) == 0)
			throw std::runtime_error("Message action missing/invalid");

		response["success"] = functions.at(request["action"].asString())(this);
	}
	catch (std::exception& e) {
		LOG->Warn(PLUGIN_NAME" :: Socket :: %s", e.what());

		response["success"] = false;
		response["message"] = e.what();

		return false;
	}

	return true;
}

void WebSocketHandler::on_message(connection_hdl hdl, ws_server::message_ptr msg)
{
	SocketRequest req;
	req.HandleRequest(msg->get_payload());

	Json::FastWriter writer;
	msg->set_payload(writer.write(req.response));
	m_server.send(hdl, msg);
}

bool WebSocketHandler::SelectSong(RString group, RString songTitle)
{
	vector<Song*> songs = SONGMAN->GetSongs(group);
	Song* song = nullptr;
	for (Song* s : songs)
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

bool WebSocketHandler::RunLua(Json::Value& req, Json::Value& res)
{
	if (!req["expression"].isString())
		return false;

	Lua* L = LUA->Get();

	if (LuaHelpers::RunExpression(L, req["expression"].asString()))
	{
		RString sRet;
		LuaHelpers::Pop(L, sRet);
		res["out"] = sRet;
	}

	lua_settop(L, 0);

	LUA->Release(L);

	return true;
}



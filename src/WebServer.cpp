//
// Created by Electromuis on 18-7-2016.
//

#include "global.h"
#include "WebServer.h"
#include "CivetServer.h"
#include "RageUtil.h"
#include "SongManager.h"
#include "json/json.h"
#include "Song.h"
#include "EnumHelper.h"

WebServer* WEBSERVER = NULL;

WebServer::~WebServer() {
    SAFE_DELETE(server);
}

WebServer::WebServer() {
    m_queueMusic = true;

    const char *options[] = {
            "document_root", ".", "listening_ports", "8080", 0};

    std::vector<std::string> cpp_options;
    for (int i=0; i<(sizeof(options)/sizeof(options[0])-1); i++) {
        cpp_options.push_back(options[i]);
    }

    server = new CivetServer(cpp_options); // <-- C++ style start

    server->addHandler("", this);
}

bool WebServer::handleGet(CivetServer *server, struct mg_connection *conn){
	const struct mg_request_info *request_info = mg_get_request_info(conn);
    RString url = request_info->request_uri;

    if(url == "/api/songs") {
        mg_printf(conn,
                  "HTTP/1.1 200 OK\r\nContent-Type: "
                          "text/plain\r\nConnection: close\r\n\r\n");

        Json::Value jsonRoot = Json::arrayValue;
        for (Song *song : SONGMAN->GetAllSongs()) {
            Json::Value SongObject = Json::objectValue;
            SongObject["Title"] = song->m_sMainTitle;
            SongObject["Artist"] = song->m_sArtist;
            SongObject["Duration"] = song->m_fMusicLengthSeconds;
            SongObject["Pack"] = song->m_sGroupName;
            SongObject["Banner"] = song->GetBannerPath();
            SongObject["BPM"] = ssprintf("%f / %f", song->m_fSpecifiedBPMMin, song->m_fSpecifiedBPMMax);

            Json::Value chartRoot = Json::arrayValue;
            for (Steps *step : song->GetAllSteps()) {
                Json::Value StepObject = Json::objectValue;
                StepObject["Difficulty"] = DifficultyToString(step->GetDifficulty());
                StepObject["Author"] = step->GetCredit();
                StepObject["Level"] = step->GetMeter();
                chartRoot.append(StepObject);
            }

            SongObject["Steps"] = chartRoot;

            jsonRoot.append(SongObject);
        }

        mg_printf(conn, jsonRoot.toStyledString().c_str());

        return true;
    } else if(url=="/api/queue"){
        mg_printf(conn,
                  "HTTP/1.1 200 OK\r\nContent-Type: "
                          "text/plain\r\nConnection: close\r\n\r\n");

        Json::Value jsonRoot = Json::arrayValue;
        for (Song *song : m_songQueue) {
            Json::Value SongObject = Json::objectValue;
            SongObject["Title"] = song->m_sMainTitle;
            SongObject["Artist"] = song->m_sArtist;
            SongObject["Pack"] = song->m_sGroupName;
            SongObject["Duration"] = song->m_fMusicLengthSeconds;

            jsonRoot.append(SongObject);
        }

        mg_printf(conn, jsonRoot.toStyledString().c_str());

        return true;
    } else if(url=="/"){
        mg_printf(conn,
                  "HTTP/1.1 302\r\n"
                          "Location: /Data/Web/index.html\r\n"
                          "Connection: close\r\n\r\n");
        return true;
    } else {
//        mg_printf(conn,
//                  "HTTP/1.1 200 OK\r\nContent-Type: "
//                          "text/plain\r\nConnection: close\r\n\r\n");
//        mg_printf(conn, request_info->request_uri);
        return false;
    }
}

bool WebServer::handlePost(CivetServer *server, struct mg_connection *conn){
    const struct mg_request_info *request_info = mg_get_request_info(conn);
    RString url = request_info->request_uri;

    RString data;
    int readBytes;
    size_t blockSize = 1024 * sizeof(char);

    char buffer[1024 * sizeof(char)];
    while ((readBytes = mg_read(conn, buffer, blockSize)) > 0) {
        data.append(buffer, 0, (readBytes / sizeof(char)));
    }

    if(url == "/api/add"){
        mg_printf(conn,
                  "HTTP/1.1 200 OK\r\nContent-Type: "
                          "text/plain\r\nConnection: close\r\n\r\n");

        Json::Value root;
        Json::Reader reader;
        if(reader.parse(data, root)) {
            Song *s = SONGMAN->FindSong(root["Pack"].asString(), root["Title"].asString());
            //Song* s = SONGMAN->GetRandomSong();
            if (s) {
                m_songQueue.push_back(s);
            }
        }

        return true;
    } else if(url=="/api/autocomplete"){
        mg_printf(conn,
                  "HTTP/1.1 200 OK\r\nContent-Type: "
                          "text/plain\r\nConnection: close\r\n\r\n");

        Json::Value jsonRoot = Json::arrayValue;
        vector<Song*> result = SONGMAN->GetAllSongs();
        SongUtil::FilterSongPointerArray(result, data);
        for (Song *song : result) {
            Json::Value SongObject = Json::objectValue;
            SongObject["Title"] = song->m_sMainTitle;
            SongObject["Artist"] = song->m_sArtist;
            SongObject["Pack"] = song->m_sGroupName;

            jsonRoot.append(SongObject);
        }

        mg_printf(conn, jsonRoot.toStyledString().c_str());

        return true;
    } else if(url=="/api/remove"){
        mg_printf(conn,
                  "HTTP/1.1 200 OK\r\nContent-Type: "
                          "text/plain\r\nConnection: close\r\n\r\n");

        std::vector<Song*>::iterator index = m_songQueue.begin()+atoi(data);
        m_songQueue.erase(index);
        return true;
    } else if(url=="/api/moveup"){
        mg_printf(conn,
                  "HTTP/1.1 200 OK\r\nContent-Type: "
                          "text/plain\r\nConnection: close\r\n\r\n");

        std::vector<Song*>::iterator index = m_songQueue.begin()+atoi(data);
        std::iter_swap(index, index-1);
        return true;
    } else if(url=="/api/movedown"){
        mg_printf(conn,
                  "HTTP/1.1 200 OK\r\nContent-Type: "
                          "text/plain\r\nConnection: close\r\n\r\n");

        std::vector<Song*>::iterator index = m_songQueue.begin()+atoi(data);
        std::iter_swap(index, index+1);
        return true;
    } else if(url=="/api/search"){
        mg_printf(conn,
                  "HTTP/1.1 200 OK\r\nContent-Type: "
                          "text/plain\r\nConnection: close\r\n\r\n");

        Json::Value jsonRoot = Json::arrayValue;
        vector<Song*> result = SONGMAN->GetAllSongs();
        SongUtil::FilterSongPointerArray(result, data);
        for (Song *song : result) {
            Json::Value SongObject = Json::objectValue;
            SongObject["Title"] = song->m_sMainTitle;
            SongObject["Artist"] = song->m_sArtist;
            SongObject["Pack"] = song->m_sGroupName;

            jsonRoot.append(SongObject);
        }

        mg_printf(conn, jsonRoot.toStyledString().c_str());

        return true;
    } else{
        mg_printf(conn, ssprintf("Bye! %s\n",request_info->request_uri));
        return false;
    }
}

Song *WebServer::GetNextSong() {
    if(!m_songQueue.empty()){
        Song *ret = m_songQueue.at(0);
        m_songQueue.erase(m_songQueue.begin());

        return ret;
    } else {
        return SONGMAN->GetRandomSong();
    }
}

void WebServer::ClearQueue() {
    m_songQueue.clear();
}



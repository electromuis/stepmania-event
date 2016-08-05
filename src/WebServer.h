//
// Created by Electromuis on 18-7-2016.
//

#ifndef STEPMANIA_WEBSERVER_H
#define STEPMANIA_WEBSERVER_H

#include "CivetServer.h"
#include "Song.h"

class WebServer : public CivetHandler {
public:
    WebServer();
    ~WebServer();

    bool handleGet(CivetServer *server, struct mg_connection *conn);
    bool handlePost(CivetServer *server, struct mg_connection *conn);
    Song *GetNextSong();
    void ClearQueue();

    bool m_queueMusic;
private:
    CivetServer *server;
    vector<Song *> m_songQueue;
};

extern WebServer* WEBSERVER;

#endif //STEPMANIA_WEBSERVER_H

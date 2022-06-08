/*******************************************************************\
| ezsockets.cpp: EzSockets Class Source                             |
|   Designed by Josh Allen, Charles Lohr and Adam Lowman.           |
|   Socket programming methods based on Charles Lohr's EZW progam.  |
|   Modified by Charles Lohr for use with Windows-Based OSes.       |
|   UDP/NON-TCP Support by Adam Lowman.                             |
\*******************************************************************/

#include "global.h"

#include "ezsockets.h"

#if defined(_MSC_VER) // We need the WinSock32 Library on Windows
#pragma comment(lib,"wsock32.lib")
#elif !defined(__MINGW32__)
#include <sys/types.h>
#include <sys/socket.h>
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#include <arpa/inet.h>
#include <netdb.h>
#endif

#if defined(_WINDOWS)
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

#if !defined(SOCKET_ERROR)
#define SOCKET_ERROR -1
#endif

//There are cases where 0 isn't a proper socket
#if !defined(SOCKET_NONE)
#define SOCKET_NONE 0
#endif

#if !defined(INVALID_SOCKET)
#define INVALID_SOCKET -1
#endif

// Returns a timeval set to the given number of milliseconds.
inline timeval timevalFromMs(unsigned int ms)
{
	timeval tv;
	tv.tv_sec = ms / 1000;
	tv.tv_usec = (ms % 1000) * 1000;
	return tv;
}

namespace
{
	struct ezs_internal
	{
		// Only necessary for Windows
#if defined(_WINDOWS)
		WSADATA wsda;
#endif

		int MAXCON;
		uintptr_t sock;
		struct sockaddr_in addr;

		struct sockaddr_in fromAddr;
		unsigned long fromAddr_len;

		// Used for Select() command
		fd_set  *scks;
		timeval *times;

		ezs_internal()
		{
			MAXCON = 5;
			memset (&addr,0,sizeof(addr)); //Clear the sockaddr_in structure

#if defined(_WINDOWS) // Windows REQUIRES WinSock Startup
			WSAStartup( MAKEWORD(1,1), &wsda );
#endif

			scks = new fd_set;
			times = new timeval;
		}
		~ezs_internal()
		{
			delete scks;
			delete times;
		}
	};
}

uint32_t EzSockets::sm_ntohl(uint32_t in)
{
	return ntohl(in);
}

uint16_t EzSockets::sm_ntohs(uint16_t in)
{
	return ntohs(in);
}

uint32_t EzSockets::sm_htonl(uint32_t in)
{
	return htonl(in);
}

uint16_t EzSockets::sm_htons(uint16_t in) {
	return htons(in);
}

EzSockets::EzSockets()
{
	ezs_internal *data = new ezs_internal();
	this->opaque = (void*)data;

	blocking = true;
	data->sock = INVALID_SOCKET;
	data->times->tv_sec = 0;
	data->times->tv_usec = 0;
	state = skDISCONNECTED;
}

EzSockets::~EzSockets()
{
	close();

	ezs_internal *data = (ezs_internal*)(this->opaque);
	delete data;
}

//Check to see if the socket has been created
bool EzSockets::check()
{
	ezs_internal *data = (ezs_internal*)(this->opaque);
	return data->sock > SOCKET_NONE;
}

bool EzSockets::create()
{
	return create(EZS_TCP, SOCK_STREAM);
}

bool EzSockets::create(EzSockets_Proto Protocol)
{
	switch(Protocol)
	{
	case EZS_TCP:
		return create(EZS_TCP, SOCK_STREAM);
	case EZS_UDP:
		return create(EZS_UDP, SOCK_DGRAM);
	default:
		return create(Protocol, SOCK_RAW);
	}
}

bool EzSockets::create(EzSockets_Proto Protocol, int Type)
{
	ezs_internal *data = (ezs_internal*)(this->opaque);
	state = skDISCONNECTED;

	int realproto = 0;
	switch (Protocol) {
		case EZS_TCP:
			realproto = IPPROTO_TCP;
			break;
		case EZS_UDP:
			realproto = IPPROTO_UDP;
			break;
		case EZS_NONE:
			realproto = IPPROTO_IP;
			break;
	}

	data->sock = socket(AF_INET, Type, realproto);
	if (data->sock > SOCKET_NONE) {
		int timeout = 1000;
		int sz = sizeof(timeout);
		setsockopt(data->sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sz);
	}
	lastCode = data->sock;
	return data->sock > SOCKET_NONE;	// Socket must be Greater than 0
}

bool EzSockets::bind(unsigned short port)
{
	if(!check())
		return false;

	ezs_internal *data = (ezs_internal*)(this->opaque);
	data->addr.sin_family      = AF_INET;
	data->addr.sin_addr.s_addr = htonl(INADDR_ANY);
	data->addr.sin_port        = htons(port);
	lastCode = ::bind(data->sock,(struct sockaddr*)&data->addr, sizeof(data->addr));
	return !lastCode;
}

bool EzSockets::listen()
{
	ezs_internal *data = (ezs_internal*)(this->opaque);
	lastCode = ::listen(data->sock, data->MAXCON);
	if (lastCode == SOCKET_ERROR)
		return false;

	state = skLISTENING;
	return true;
}

#if defined(WIN32)
typedef int socklen_t;
#endif

bool EzSockets::accept(EzSockets& socket)
{
	if (!blocking && !CanRead())
		return false;

	#if defined(HAVE_INET_NTOP)
		char buf[INET_ADDRSTRLEN];

		inet_ntop(AF_INET, &addr.sin_addr, buf, INET_ADDRSTRLEN);
		address = buf;

	#elif defined(HAVE_INET_NTOA)
		address = inet_ntoa(addr.sin_addr);
	#endif

	int length = sizeof(socket);
	
	ezs_internal *data = (ezs_internal*)(this->opaque);
	ezs_internal *sdata = (ezs_internal*)(socket.opaque);
	sdata->sock = ::accept(data->sock,(struct sockaddr*) &sdata->addr,
						   (socklen_t*) &length);

	lastCode = sdata->sock;

	if ( sdata->sock == SOCKET_ERROR )
		return false;

	socket.state = skCONNECTED;
	return true;
}

void EzSockets::close()
{
	state = skDISCONNECTED;
	inBuffer = "";
	outBuffer = "";

	ezs_internal *data = (ezs_internal*)(this->opaque);
#if defined(WIN32) // The close socket command is different in Windows
	::closesocket(data->sock);
#else
	::close(data->sock);
#endif
}

long EzSockets::uAddr()
{
	ezs_internal *data = (ezs_internal*)(this->opaque);
	return data->addr.sin_addr.s_addr;
}

bool EzSockets::connect(const std::string& host, unsigned short port)
{
	if (!check())
	{
		return false;
	}

	struct hostent* phe;
	phe = gethostbyname(host.c_str());
	if (phe == nullptr)
	{
		return false;
	}

	ezs_internal *data = (ezs_internal*)(this->opaque);
	memcpy(&data->addr.sin_addr, phe->h_addr, sizeof(struct in_addr));

	data->addr.sin_family = AF_INET;
	data->addr.sin_port   = htons(port);

	if(::connect(data->sock, (struct sockaddr*)&data->addr, sizeof(data->addr)) == SOCKET_ERROR)
	{
		return false;
	}

	state = skCONNECTED;
	return true;
}

inline bool checkCanRead(int sock, timeval& timeout)
{
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET((unsigned)sock, &fds);

	return select(sock+1, &fds, nullptr, nullptr, &timeout) > 0;
}

bool EzSockets::CanRead()
{
	ezs_internal *data = (ezs_internal*)(this->opaque);
	return checkCanRead(data->sock, *data->times);
}

bool EzSockets::CanRead(unsigned int msTimeout)
{
	ezs_internal *data = (ezs_internal*)(this->opaque);
	timeval tv = timevalFromMs(msTimeout);
	return checkCanRead(data->sock, tv);
}

bool EzSockets::IsError()
{
	if (state == skERROR)
		return true;

	ezs_internal *data = (ezs_internal*)(this->opaque);
	FD_ZERO(data->scks);
	FD_SET((unsigned)data->sock, data->scks);

	if (select(data->sock+1, nullptr, nullptr, data->scks, data->times) >=0 )
		return false;

	state = skERROR;
	return true;
}

inline bool checkCanWrite(int sock, timeval& timeout)
{
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET((unsigned)sock, &fds);

	return select(sock+1, nullptr, &fds, nullptr, &timeout) > 0;
}

bool EzSockets::CanWrite()
{
	ezs_internal *data = (ezs_internal*)(this->opaque);
	return checkCanWrite(data->sock, *data->times);
}

bool EzSockets::CanWrite(unsigned int msTimeout)
{
	ezs_internal *data = (ezs_internal*)(this->opaque);
	timeval tv = timevalFromMs(msTimeout);
	return checkCanWrite(data->sock, tv);
}

void EzSockets::update()
{
	if (IsError()) //If socket is in error, don't bother.
		return;

	while (CanRead() && !IsError()) //Check for Reading
		if (pUpdateRead() < 1)
			break;

	if (CanWrite() && (outBuffer.length()>0))
		pUpdateWrite();
}

uint32_t EzSockets::getAddress()
{
	ezs_internal *data = (ezs_internal*)(this->opaque);
	sockaddr_in &s = data->fromAddr;

#if defined(_WINDOWS)
	return ntohl(s.sin_addr.S_un.S_addr);
#else
	return ntohl(s.sin_addr.s_addr);
#endif
}


/*********************\
|   Raw Data System   |
\*********************/
void EzSockets::SendData(const string& outData)
{
	outBuffer.append(outData);
	if(blocking)
		while ((outBuffer.length()>0) && !IsError())
			pUpdateWrite();
	else
		update();
}

void EzSockets::SendData(const char *data, unsigned int bytes)
{
	outBuffer.append(data, bytes);
	if(blocking)
		while ((outBuffer.length()>0) && !IsError())
			pUpdateWrite();
	else
		update();
}

int EzSockets::ReadData(char *data, unsigned int bytes)
{	
	int bytesRead = PeekData(data,bytes);
	inBuffer = inBuffer.substr(bytesRead);
	return bytesRead;
}

int EzSockets::PeekData(char *data, unsigned int bytes)
{
	if (blocking)
		while ((inBuffer.length()<bytes) && !IsError())
			pUpdateRead();
	else
		while (CanRead() && !IsError())
			if (pUpdateRead()<1)
				break;

	int bytesRead = bytes;
	if (inBuffer.length()<bytes)
		bytesRead = inBuffer.length();
	memcpy(data,inBuffer.c_str(), bytesRead);

	return bytesRead;
}


/**********************************\
|   Packet/Structure Data System   |
\**********************************/
void EzSockets::SendPack(const char *data, unsigned int bytes)
{
	unsigned int SendSize = htonl(bytes);
	outBuffer.append( (const char *) & SendSize, 4);	//Add size to buffer, but don't send yet.
	SendData(data, bytes);
}

int EzSockets::ReadPack(char *data, unsigned int max)
{
	int size = PeekPack(data, max);

	if (size != -1)
		inBuffer = inBuffer.substr(static_cast<size_t>(size) + 4);

	return size;
}

int EzSockets::PeekPack(char *data, unsigned int max)
{
	if (CanRead())
		pUpdateRead();

	if (blocking)
	{
		while ((inBuffer.length()<4) && !IsError())
			pUpdateRead();

		if (IsError())
			return -1;
	}

	if (inBuffer.length()<4)
		return -1;

	unsigned int size;
	PeekData((char*)&size, 4);
	size = ntohl(size);

	if (blocking)
		while (inBuffer.length()<(static_cast<size_t>(size) + 4) && !IsError())
			pUpdateRead();
	else
		if (inBuffer.length()<(static_cast<size_t>(size) + 4) || inBuffer.length()<=4)
			return -1;

	if (IsError())
		return -1; 
	// What if we get disconnected while waiting for data?

	string tBuff(inBuffer.substr(4, size));
	if (tBuff.length() > max)
		tBuff.substr(0, max);

	memcpy (data, tBuff.c_str(),tBuff.length());
	return size;
}


/*****************************************\
|   Null Terminating String Data System   |
\*****************************************/
void EzSockets::SendStr(const string& data, char delim)
{
	char tDr[1];
	tDr[0] = delim;
	SendData(data.c_str(), data.length());
	SendData(tDr, 1);
}

int EzSockets::ReadStr(string& data, char delim)
{
	int t = PeekStr(data, delim);
	if (t >= 0)
		inBuffer = inBuffer.substr(t+1);
	return t;
}

int EzSockets::PeekStr(string& data, char delim)
{
	int t = inBuffer.find(delim,0);
	if (blocking)
	{
		while (t == -1 && !IsError())
		{
			pUpdateRead();
			t = inBuffer.find(delim, 0);
		}
		data = inBuffer.substr(0, t);
	}

	if(t >= 0)
		data = inBuffer.substr(0, t);
	return t;
}


/************************\
|   Stream Data System   |
\************************/
istream& operator>>(istream &is, EzSockets& obj)
{
	string writeString;
	obj.SendStr(writeString);
	is >> writeString;
	return is;
}

ostream& operator<<(ostream &os, EzSockets &obj)
{
	string readString;
	obj.ReadStr(readString);
	os << readString;
	return os;
}


/**************************\
|   Internal Data System   |
\**************************/
int EzSockets::pUpdateRead()
{
	char tempData[1024];
	int bytes = pReadData(tempData);
	int sockErr = 0;

	if (bytes > 0)
		inBuffer.append(tempData, bytes);
	else if (bytes <= 0)
	{
		/* To get her I think CanRead was called at least once.
		So if length equals 0 and can read says there is data than
		the socket was closed.*/
		sockErr = WSAGetLastError();
		state = skERROR;
	}
	return bytes;
}

int EzSockets::pUpdateWrite()
{
	int bytes = pWriteData(outBuffer.c_str(), outBuffer.length());

	if (bytes > 0)
		outBuffer = outBuffer.substr(bytes);
	else if (bytes < 0)
		state = skERROR;
	return bytes;
}


int EzSockets::pReadData(char* data)
{
	ezs_internal *sdata = (ezs_internal*)(this->opaque);
	if (state == skCONNECTED || state == skLISTENING)
		return recv(sdata->sock, data, 1024, 0);

	sdata->fromAddr_len = sizeof(sockaddr_in);
	return recvfrom(sdata->sock, data, 1024, 0, (sockaddr*)&sdata->fromAddr,
					(socklen_t*)&sdata->fromAddr_len);
}

int EzSockets::pWriteData(const char* data, int dataSize)
{
	ezs_internal *sdata = (ezs_internal*)(this->opaque);
	return send(sdata->sock, data, dataSize, 0);
}

/* 
 * (c) 2003-2004 Josh Allen, Charles Lohr, and Adam Lowman
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

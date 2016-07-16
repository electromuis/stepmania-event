#ifndef TOURNAMENT_MANAGER_MSG_H
#define TOURNAMENT_MANAGER_MSG_H

typedef unsigned char uchar;

// PACKET STRUCTURE:
//    I32 messageID;

// PLAYER TABLE STRUCTURE:
//    I32 numPlayers;
//    players[numPlayers]
//    {
//      I32 nameLen;
//      I8[nameLen] name;
//      I32 score;
//      I32 surviveTimeMS;
//    }
//    I32 placementType;
//    I32 numRules;
//    rules[numRules]
//    {
//      I32 perRound;
//      I32 remaining;
//      I32 toSelect;
//    }
//    I32 disabledPadBits;

// SCORE UPDATE STRUCTURE:
//    I32 numPlayers;
//    players[numPlayers]
//    {
//      I32 nameLen;
//      I8[nameLen] name;
//      I32 score;
//      I32 surviveTimeMS;
//    }

enum MessageIDs
{
	TMM_PING_MESSAGE = 0x524,
	TMM_PLAYER_TABLE = 0x525,
	TMM_SCORE_UPDATE = 0x526,
};

enum NoScoreTime
{
	TMM_NO_SCORE = -1,
	TMM_NO_TIME = -1,
};

enum PortNumber
{
	TMM_PORT_NUMBER = 8766,
};

inline int Read32(const uchar*& p, const uchar* end)
{
	if (p == NULL || p + 4 > end) { p = NULL; return 0; }
	int v = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
	p += 4;
	return v;
}

inline void Write32(std::vector<uchar>& out, int v)
{
	out.push_back((v >> 24) & 0xFF);
	out.push_back((v >> 16) & 0xFF);
	out.push_back((v >> 8) & 0xFF);
	out.push_back((v >> 0) & 0xFF);
}

inline std::string ReadStr(const uchar*& p, int len, const uchar* end)
{
	if (p == NULL || p + len > end) { p = NULL; return std::string(); }
	std::string v((const char*)p, len);
	p += len;
	return v;
}

inline void WriteStr(std::vector<uchar>& out, const char* str, int len)
{
	Write32(out, len);
	for (int i = 0; i<len; ++i) out.push_back(str[i]);
}

inline void WritePlayer(std::vector<uchar>& out, const char* name, int score, int time)
{
	int len = (int)strlen(name);
	WriteStr(out, name, len);
	Write32(out, score);
	Write32(out, time);
}

#endif // TOURNAMENT_MANAGER_MSG_H
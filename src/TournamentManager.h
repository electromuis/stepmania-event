#ifndef TournamentManager_H
#define TournamentManager_H

#include "PlayerNumber.h"

struct lua_State;

class EzSockets;
class TournamentManager
{
public:
	struct TournamentPlayer
	{
		RString name;
		int score, time;
	};
	struct SelectRule
	{
		int perRound, remaining, toSelect;
	};

	TournamentManager();
	~TournamentManager();

	void Activate();
	void Terminate();

	void Update(float fDeltaTime);

	void StartListening();
	void SendPing();
	void SendScores(const int* scores, const int* times, int count);
	bool SendMessage(void* data, int size);
	void AcceptMessage(const void* data, int size);

	void SelectPlayers();
	void LockSelection(bool lock);

	RString GetPlayerName(PlayerNumber pn) const;
	bool IsActive() const;

	// Lua
	void PushSelf(lua_State *L);

private:
	void Reset();
	vector<TournamentPlayer> m_PlayerTable;
	vector<SelectRule> m_SelectRules;
	RString m_CurrentPlayers[NUM_PLAYERS];
	EzSockets* m_Listener, *m_Socket;
	bool m_SelectionLocked, m_IsActive, m_IsConnected;
	bool m_PlaceTowardsCenter;
	float m_ConnectionTimer;
	bool m_DisabledPads[NUM_PLAYERS];
};

extern TournamentManager* TOURNAMENTMAN; // global and accessable from anywhere in our program

#endif

/*
* (c) 2013 Bram van de Wetering
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
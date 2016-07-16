#include "global.h"
#include "TournamentManager.h"
#include "ScreenManager.h"
#include "GameState.h"
#include "Foreach.h"
#include "ProfileManager.h"
#include "PrefsManager.h"
#include "Steps.h"
#include "ezsockets.h"
#include "MessageManager.h"
#include "LuaReference.h"

#ifdef SendMessage
#undef SendMessage
#endif

typedef unsigned char uchar;
#include "TournamentManagerMsg.h"

// Defined in GraphicsWindow.cpp
extern bool SendWindowMessage(void* data, int size);

// ============================================================================

TournamentManager* TOURNAMENTMAN = NULL; // global object accessable from anywhere in the program

TournamentManager::TournamentManager()
{
	m_Listener = NULL;
	m_Socket = NULL;
	Reset();

	// Register with Lua.
	{
		Lua *L = LUA->Get();
		lua_pushstring(L, "TOURNAMENTMAN");
		this->PushSelf(L);
		lua_settable(L, LUA_GLOBALSINDEX);
		LUA->Release(L);
	}
}

TournamentManager::~TournamentManager()
{
	Reset();

	LUA->UnsetGlobal("TOURNAMENTMAN");
}

void TournamentManager::Reset()
{
	m_IsConnected = false;
	m_IsActive = false;
	m_SelectionLocked = false;
	m_PlaceTowardsCenter = false;
	m_ConnectionTimer = 0.f;

	m_PlayerTable.clear();
	for (int i = 0; i < NUM_PLAYERS; ++i)
	{
		m_CurrentPlayers[i].clear();
		m_DisabledPads[i] = false;
	}

	SAFE_DELETE(m_Listener);
	SAFE_DELETE(m_Socket);
}

void TournamentManager::Activate()
{
	if (m_IsActive) return;

	Reset();
	StartListening();
	m_IsActive = true;
}

void TournamentManager::Terminate()
{
	if (!m_IsActive) return;

	Reset();
}

void TournamentManager::Update(float fDeltaTime)
{
	if (!m_IsActive) return;

	// We only need to update a few times per second.
	m_ConnectionTimer += fDeltaTime;
	if (m_ConnectionTimer < 0.5f) return;
	m_ConnectionTimer = 0.f;

	if (m_IsConnected)
	{
		// Check if the connection is still valid, and if new packets have arrived.
		if ((!m_Socket) || m_Socket->IsError())
		{
			SCREENMAN->SystemMessage("Tournament manager: disconnected.");
			StartListening();
		}
		else
		{
			while (m_Socket->DataAvailable())
			{
				int size = m_Socket->PeekPack(NULL, 0);
				if (size <= 0) break;
				vector<uchar> data(size);
				uchar* p = &data[0];
				size = m_Socket->ReadPack((char*)p, size);
				AcceptMessage(p, size);
			}
		}
	}
	else if (m_Listener)
	{
		// If we are not connected to the tournament manager.
		// Check if the listener is ready to accept an incomming connection.
		if (m_Listener->accept(*m_Socket))
		{
			RString m = "";
			m = ssprintf("Tournament manager: connected to %s.", m.c_str());
			SCREENMAN->SystemMessage(m);
			SAFE_DELETE(m_Listener);
			m_IsConnected = true;
		}
	}
}

void TournamentManager::StartListening()
{
	m_IsConnected = false;

	SAFE_DELETE(m_Listener);
	SAFE_DELETE(m_Socket);

	m_Listener = new EzSockets;
	m_Listener->blocking = false;

	m_Socket = new EzSockets;
	m_Socket->blocking = false;

	if (m_Listener->create() && m_Listener->bind(TMM_PORT_NUMBER) && m_Listener->listen())
	{
		SCREENMAN->SystemMessage("Tournament manager: listening for connections...");
	}
	else
	{
		SCREENMAN->SystemMessage("Tournament manager: failed to open a listening socket!");
		SAFE_DELETE(m_Listener);
		SAFE_DELETE(m_Socket);
	}
}

void TournamentManager::SendPing()
{
	if (!m_IsActive) return;

	std::vector<uchar> out;
	Write32(out, TMM_PING_MESSAGE);
	if (!SendMessage(&out[0], out.size()))
	{
		SCREENMAN->SystemMessage("Tournament manager: failed to send ping!");
	}
}

void TournamentManager::SendScores(const int* scores, const int* times, int count)
{
	if (!m_IsActive) return;

	vector<TournamentPlayer> players;
	for (int i = 0; i<count && i<NUM_PLAYERS; ++i)
	{
		RString name = m_CurrentPlayers[i];
		if (!name.empty())
		{
			TournamentPlayer player = { name, scores[i], times[i] };
			players.push_back(player);
		}
	}

	if (players.size())
	{
		vector<uchar> out;
		Write32(out, TMM_SCORE_UPDATE);
		Write32(out, (int)players.size());
		for (size_t i = 0; i<players.size(); ++i)
		{
			WritePlayer(out, players[i].name.c_str(),
				players[i].score, players[i].time);
		}

		if (!SendMessage(&out[0], out.size()))
		{
			SCREENMAN->SystemMessage("Tournament manager: failed to send scores!");
		}
	}
}

bool TournamentManager::SendMessage(void* data, int size)
{
	if (!m_IsActive) return false;

	// If the socket is valid, we send messages over the network.
	if (m_IsConnected && m_Socket)
	{
		m_Socket->SendPack((const char*)data, size);
		return !m_Socket->IsError();
	}

	return false;
}

void TournamentManager::AcceptMessage(const void* data, int size)
{
	if (!m_IsActive) return;

	const uchar* p = (const uchar*)data, *end = p + size;
	int id = Read32(p, end);

	// Check if the message is a ping message.
	if (id == TMM_PING_MESSAGE)
	{
		SCREENMAN->SystemMessage("Tournament manager: received ping!");
	}

	// Check if the message is a player table.
	if (id == TMM_PLAYER_TABLE)
	{
		m_PlayerTable.clear();
		m_SelectRules.clear();

		// Add the players from the message.
		int numPlayers = Read32(p, end);
		for (int i = 0; i<numPlayers; ++i)
		{
			int nameLen = Read32(p, end);
			RString name = ReadStr(p, nameLen, end);
			int score = Read32(p, end);
			int time = Read32(p, end);
			TournamentPlayer pl = { name, score, time };
			m_PlayerTable.push_back(pl);
		}

		// Add the placementType from the message.
		int placementType = Read32(p, end);
		m_PlaceTowardsCenter = (placementType == 1);

		// Add the rules from the message.
		int numRules = Read32(p, end);
		for (int i = 0; i<numRules; ++i)
		{
			SelectRule rule;
			rule.perRound = Read32(p, end);
			rule.remaining = Read32(p, end);
			rule.toSelect = Read32(p, end);
			m_SelectRules.push_back(rule);
		}

		// Read the disabled pads from the message.
		unsigned int disabledPads = Read32(p, end);
		for (int i = 0; i < NUM_PLAYERS; ++i)
		{
			m_DisabledPads[i] = (disabledPads & (1 << i));
		}

		SelectPlayers();
	}
}

void TournamentManager::SelectPlayers()
{
	if (!m_IsActive) return;

	if (!m_SelectionLocked)
	{
		// Clear currently selected players.
		for (int i = 0; i < NUM_PLAYERS; ++i)
		{
			m_CurrentPlayers[i].clear();
		}

		// Determine the number of slots that can accept players.
		int availableSlots = 0;
		int totalSlots = GAMESTATE ? GAMESTATE->GetNumPlayersEnabled() : NUM_PLAYERS;
		for (int i = 0; i < totalSlots; ++i)
		{
			if (!m_DisabledPads[i]) ++availableSlots;
		}

		// Pick players from the player table that still have to play.
		vector<RString> players;
		for (auto& p : m_PlayerTable)
			if (p.score == TMM_NO_SCORE)
				players.push_back(p.name);
		int remainingPlayers = (int)players.size();

		// Apply the selection rules.
		int numSelected = NUM_PLAYERS;
		for (size_t i = 0; i<m_SelectRules.size(); ++i)
		{
			const SelectRule& rule = m_SelectRules[i];
			if (availableSlots == rule.perRound && remainingPlayers == rule.remaining)
				numSelected = rule.toSelect;
		}

		// Clamping in case we have wacky selection rules (we don't know).
		if (numSelected > remainingPlayers) numSelected = remainingPlayers;
		if (numSelected > availableSlots) numSelected = availableSlots;
		if (numSelected < 0) numSelected = 0;

		// Truncate the player list to the number of players we need.
		players.resize(numSelected);

		// Determine the placement.
		if (m_PlaceTowardsCenter)
		{
			// Placement starts at the center and works outward.
			std::reverse(players.begin(), players.end());
			int center = (totalSlots - 1) / 2, offset = 1;
			for (int slotI = center, pI = 0; slotI >= 0 && slotI < totalSlots && pI < numSelected;)
			{
				if (!m_DisabledPads[slotI])
				{
					m_CurrentPlayers[slotI] = players[pI++];
				}
				slotI = (slotI <= center) ? (slotI + offset) : (slotI - offset);
				++offset;
			}
		}
		else
		{
			// Linear placement from left to right.
			for (int slotI = 0, pI = 0; slotI < totalSlots && pI < numSelected;)
			{
				if (!m_DisabledPads[slotI])
				{
					m_CurrentPlayers[slotI] = players[pI++];
				}
				++slotI;
			}
		}

		Message message("");
		message.SetName("TournamentUpdate");

		MESSAGEMAN->Broadcast(message);
	}
}

void TournamentManager::LockSelection(bool lock)
{
	if (!m_IsActive) return;

	m_SelectionLocked = lock;
}

RString TournamentManager::GetPlayerName(PlayerNumber pn) const
{
	if (m_IsActive && pn >= 0 && pn < NUM_PLAYERS)
		return m_CurrentPlayers[pn];

	return "";
}

bool TournamentManager::IsActive() const
{
	return m_IsActive;
}

// lua start
#include "LuaBinding.h"

class LunaTournamentManager : public Luna<TournamentManager>
{
public:
	DEFINE_METHOD(IsActive, IsActive())

	LunaTournamentManager() {
		ADD_METHOD(GetPlayerName);
		ADD_METHOD(IsActive);
		ADD_METHOD(Activate);
		ADD_METHOD(Terminate);
	}

	static int GetPlayerName(T* p, lua_State *L)
	{
		PlayerNumber pn = Enum::Check<PlayerNumber>(L, 1);
		lua_pushstring(L, p->GetPlayerName(pn));
		return 1;
	}

	static int Activate(T* p, lua_State *L)
	{
		p->Activate();
		COMMON_RETURN_SELF;
	}

	static int Terminate(T* p, lua_State *L)
	{
		p->Terminate();
		COMMON_RETURN_SELF;
	}
};

LUA_REGISTER_CLASS(TournamentManager)
// lua end

/*
* (c) 2001-2004 Chris Danford
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
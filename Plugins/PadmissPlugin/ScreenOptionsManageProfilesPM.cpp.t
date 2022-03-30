#include "global.h"
#include "ScreenOptionsManageProfilesPM.h"
#include "Actor.h"
#include "ProfileManager.h"
#include "OptionRowHandler.h"
#include "GameState.h"
#include "PluginManager.h"
#include <ScreenTextEntry.h>
#include <LocalizedString.h>
#include "LuaManager.h"
#include "MessageManager.h"
#include "PadmissPlugin.h"
#include "ScreenManager.h"

#define SHOW_CREATE_NEW (!PROFILEMAN->FixedProfiles())

AutoScreenMessage(SM_EnterUsername);
AutoScreenMessage(SM_BackFromUsername);
AutoScreenMessage(SM_BackFromPassword);

static LocalizedString DOWNLOAD_PROFILE_USERNAME("ScreenOptionsManageProfiles", "Enter your username.");
static LocalizedString DOWNLOAD_PROFILE_PASSWORD("ScreenOptionsManageProfiles", "Enter your password.");

Screen* CreateScreenOptionsManageProfilesPM( const RString &sName ) {
	//LuaThreadVariable var( "LoadingScreen", *screenName);

	//void* screenLocation = malloc(sizeof(ScreenOptionsManageProfilesPM));
	//Screen* pRet = new(screenLocation) ScreenOptionsManageProfilesPM();

	Screen* pRet = new ScreenOptionsManageProfilesPM();
	//RString screenName = "";

	Screen::InitScreen( pRet );
	return pRet;
}

//RegisterScreenClass register_ScreenOptionsManageProfilesPM("ScreenOptionsManageProfilesPM", CreateScreenOptionsManageProfilesPM);

ScreenOptionsManageProfilesPM::ScreenOptionsManageProfilesPM()
{
	SetName("ScreenOptionsManageProfilesPM");
}

ScreenOptionsManageProfilesPM::~ScreenOptionsManageProfilesPM()
{
	
}

void ScreenOptionsManageProfilesPM::BeginScreen()
{
	// FIXME
	// int iIndex = 0;
	vector<OptionRowHandler*> OptionRowHandlers;

	if (SHOW_CREATE_NEW)
	{
		OptionRowHandler* pHand = OptionRowHandlerUtil::Make(ParseCommands(ssprintf("gamecommand;screen,%s;name,dummy", m_sName.c_str())));
		OptionRowDefinition& def = pHand->m_Def;
		def.m_layoutType = LAYOUT_SHOW_ALL_IN_ROW;
		def.m_bAllowThemeTitle = true;
		def.m_bAllowThemeItems = false;
		def.m_sName = "Create New Profile";
		def.m_sExplanationName = "Create New Profile";
		OptionRowHandlers.push_back(pHand);

		// FIXME
		// gc.Load( iIndex++,  );
	}

	PROFILEMAN->GetLocalProfileIDs(m_vsLocalProfileID);

	for (RString const& s : m_vsLocalProfileID)
	{
		Profile* pProfile = PROFILEMAN->GetLocalProfile(s);
		ASSERT(pProfile != nullptr);

		RString sCommand = ssprintf("gamecommand;screen,ScreenOptionsCustomizeProfile;profileid,%s;name,dummy", s.c_str());
		OptionRowHandler* pHand = OptionRowHandlerUtil::Make(ParseCommands(sCommand));
		OptionRowDefinition& def = pHand->m_Def;
		def.m_layoutType = LAYOUT_SHOW_ALL_IN_ROW;
		def.m_bAllowThemeTitle = false;
		def.m_bAllowThemeItems = false;
		def.m_sName = pProfile->m_sDisplayName;
		def.m_sExplanationName = "Select Profile";

		PlayerNumber pn = PLAYER_INVALID;
		FOREACH_PlayerNumber(p)
			if (s == ProfileManager::m_sDefaultLocalProfileID[p].Get())
				pn = p;
		if (pn != PLAYER_INVALID) {
			void* stringAddress = PLUGINMAN->AppAllocate(sizeof(RString));
			RString* dummy = new(stringAddress) RString(PlayerNumberToLocalizedString(pn));
			def.m_vsChoices.push_back(*dummy);
		}
		OptionRowHandlers.push_back(pHand);

		// FIXME
		// gc.Load( iIndex++,  );
	}

	RString* dummy = APP_PTR(RString, "");
	
	// Add dummy item to act like the download button
	m_vsLocalProfileID.push_back(*dummy);
	
	OptionRowHandler* pHand2 = OptionRowHandlerUtil::Make(ParseCommands(ssprintf("gamecommand;screen,%s;name,dummy", m_sName.c_str())));
	OptionRowDefinition& def2 = pHand2->m_Def;
	def2.m_layoutType = LAYOUT_SHOW_ALL_IN_ROW;
	def2.m_bAllowThemeTitle = false;
	def2.m_bAllowThemeItems = false;
	def2.m_sName = "Download Profile";
	def2.m_sExplanationName = "Download Profile";
	OptionRowHandlers.push_back(pHand2);
	

	ScreenOptions::InitMenu(OptionRowHandlers);

	// Save sEditLocalProfileID before calling ScreenOptions::BeginScreen, because it will get clobbered.
	RString sEditLocalProfileID = GAMESTATE->m_sEditLocalProfileID;

	ScreenOptions::BeginScreen();

	// select the last chosen profile
	if (!sEditLocalProfileID.empty())
	{
		vector<RString>::const_iterator iter = find(m_vsLocalProfileID.begin(), m_vsLocalProfileID.end(), sEditLocalProfileID);
		if (iter != m_vsLocalProfileID.end())
		{
			int iIndex = iter - m_vsLocalProfileID.begin();
			this->MoveRowAbsolute(PLAYER_1, 1 + iIndex);
		}
	}
	else if (!m_vsLocalProfileID.empty())
	{
		// select the first item below "create new"
		this->MoveRowAbsolute(PLAYER_1, 1);
	}

	AfterChangeRow(PLAYER_1);
}

void ScreenOptionsManageProfilesPM::ProcessMenuStart(const InputEventPlus& input)
{
	if (IsTransitioning())
		return;

	int iCurRow = m_iCurrentRow[GAMESTATE->GetMasterPlayerNumber()];
	OptionRow& row = *m_pRows[iCurRow];

	if (iCurRow == m_pRows.size() - 2) {

		//MESSAGEMAN->Broadcast("DownloadProfile");
		HandleScreenMessage(SM_EnterUsername);

		// Make sure out dummy item gets skipped
		return;
	}

	// Parent list thinks it's 1 short
	ScreenOptionsManageProfiles::ProcessMenuStart(input);
}

std::string lastUsername = "";
std::string lastPassword = "";


void ScreenOptionsManageProfilesPM::HandleScreenMessage(const ScreenMessage SM)
{
	if (SM == SM_EnterUsername)
	{
		ScreenTextEntry::TextEntry(
			SM_BackFromUsername,
			DOWNLOAD_PROFILE_USERNAME,
			lastUsername,
			255);

		return;
	}
	else if (SM == SM_BackFromUsername)
	{
		lastUsername = ScreenTextEntry::s_sLastAnswer;

		ScreenTextEntry::TextEntry(
			SM_BackFromPassword,
			DOWNLOAD_PROFILE_PASSWORD,
			"",
			255);

		return;
	}
	else if (SM == SM_BackFromPassword)
	{
		lastPassword = ScreenTextEntry::s_sLastAnswer;

		PadmissPlugin* p = (PadmissPlugin *)PLUGINMAN->GetPlugin(PLUGIN_NAME)->GetPlugin();
		if (p->Login(lastUsername, lastPassword)) {
			if (p->DownloadProfile()) {
				SCREENMAN->SetNewScreen(this->m_sName);
			}
		}

		return;
	}

	ScreenOptionsManageProfiles::HandleScreenMessage(SM);
}

/*
 * (c) 2002-2004 Chris Danford
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, includin
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

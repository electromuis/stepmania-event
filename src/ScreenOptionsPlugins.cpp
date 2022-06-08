#include "global.h"
#include "ScreenOptionsPlugins.h"
#include "PluginManager.h"
#include "OptionRowHandler.h"
#include "GameState.h"

REGISTER_SCREEN_CLASS( ScreenOptionsPlugins );

void ScreenOptionsPlugins::Init()
{
	ScreenOptions::Init();

	SetNavigation( NAV_THREE_KEY_MENU );
	SetInputMode( INPUTMODE_SHARE_CURSOR );
}

void ScreenOptionsPlugins::BeginScreen()
{
	vector<OptionRowHandler*> OptionRowHandlers;

	for (int p = 0; p < PLUGINMAN->GetNumPlugins(); p++)
	{
		auto plugin = PLUGINMAN->GetPlugin(p);
		if (!plugin)
			continue;

		OptionRowHandler* pHand = nullptr;
		
		RString metric = ssprintf("Line%s", plugin->GetName().c_str());
		if (THEME->HasMetric(m_sName, metric))
		{
			RString sRowCommands = THEME->GetMetric(m_sName, metric);
			Commands cmds;
			ParseCommands(sRowCommands, cmds, false);
			pHand = OptionRowHandlerUtil::Make(cmds);
		}
		else
		{
			pHand = OptionRowHandlerUtil::MakeNull();
		}

		

		OptionRowDefinition& def = pHand->m_Def;
		def.m_layoutType = LAYOUT_SHOW_ALL_IN_ROW;

		if(plugin->IsLoaded())
			def.m_vsChoices.push_back("Loaded");
		else
			def.m_vsChoices.push_back("Not loaded");

		if(plugin->HasError())
			def.m_vsChoices.push_back("Loading failed");

		def.m_bAllowThemeTitle = false;
		def.m_bAllowThemeItems = false;
		def.m_bAllowExplanation = false;
		def.m_sName = plugin->GetName();

		OptionRowHandlers.push_back(pHand);
	}

	ScreenOptions::InitMenu(OptionRowHandlers);
	ScreenOptions::BeginScreen();
}

void ScreenOptionsPlugins::ProcessMenuStart( const InputEventPlus& e )
{
	ScreenOptions::ProcessMenuStart(e);
}

void ScreenOptionsPlugins::ImportOptions( int /* iRow */, const vector<PlayerNumber> & /* vpns */ )
{

}

void ScreenOptionsPlugins::ExportOptions( int /* iRow */, const vector<PlayerNumber> & /* vpns */ )
{

}

void ScreenOptionsPlugins::AfterChangeValueOrRow(PlayerNumber pn)
{
	ScreenOptions::AfterChangeValueOrRow(pn);

	int iCurRow = m_iCurrentRow[GAMESTATE->GetMasterPlayerNumber()];
	if (iCurRow >= PLUGINMAN->GetNumPlugins())
		return;

	auto plugin = PLUGINMAN->GetPlugin(iCurRow);
	if (!plugin)
		return;

	RString text = "Name: " + plugin->GetName() + "\n";
	text.append("Version: " + plugin->GetVersion() + "\n");
	text.append("Author: " + plugin->GetAuthor());

	m_textExplanationTogether.FinishTweening();
	ON_COMMAND(m_textExplanationTogether);
	m_textExplanationTogether.SetText(text);
}

/*
 * (c) 2002-2004 Chris Danford
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

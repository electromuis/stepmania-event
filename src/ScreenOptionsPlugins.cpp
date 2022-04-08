#include "global.h"
#include "ScreenOptionsManageProfiles.h"

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
	ScreenOptions::BeginScreen();
}

void ScreenOptionsManageProfiles::HandleScreenMessage( const ScreenMessage SM )
{
	if( SM == SM_LoseFocus )
	{
		this->PlayCommand( "ScreenLoseFocus" );
	}
	else if( SM == SM_GainFocus )
	{
		this->PlayCommand( "ScreenGainFocus" );
	}

	ScreenOptions::HandleScreenMessage( SM );
}

void ScreenOptionsManageProfiles::ProcessMenuStart( const InputEventPlus & )
{
	if( IsTransitioning() )
		return;

	int iCurRow = m_iCurrentRow[GAMESTATE->GetMasterPlayerNumber()];
	OptionRow &row = *m_pRows[iCurRow];
	
	
}

void ScreenOptionsManageProfiles::ImportOptions( int /* iRow */, const vector<PlayerNumber> & /* vpns */ )
{

}

void ScreenOptionsManageProfiles::ExportOptions( int /* iRow */, const vector<PlayerNumber> & /* vpns */ )
{

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

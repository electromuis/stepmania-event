#include "global.h"

#include "ScoreDisplayPercentage.h"
#include "ThemeManager.h"
#include "StatsManager.h"
#include "PlayerState.h"

ScoreDisplayPercentage::ScoreDisplayPercentage()
{
	// todo: allow ScoreDisplayPercentage to have its own frame? -aj
	m_sprFrame.Load( THEME->GetPathG("ScoreDisplayNormal","frame") );
	this->AddChild( m_sprFrame );

	m_Percent.SetName( "ScoreDisplayPercentage Percent" );
	this->AddChild( &m_Percent );
}

void ScoreDisplayPercentage::Init( const PlayerState* pPlayerState, const PlayerStageStats* pPlayerStageStats ) 
{
	ScoreDisplay::Init( pPlayerState, pPlayerStageStats );

	m_Percent.Load( pPlayerState, pPlayerStageStats,
		"ScoreDisplayPercentage Percent", true );
}

/*
 * (c) 2003 Glenn Maynard
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
 * 
 * (c) 2016- Electromuis, Anton Grootes
 * This branch of https://github.com/stepmania/stepmania
 * will from here on out be released as GPL v3 (wich converts from the previous MIT license)
 */

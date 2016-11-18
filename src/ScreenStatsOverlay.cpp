#include "global.h"
#include "ScreenStatsOverlay.h"
#include "RageMath.hpp"
#include "ActorUtil.h"
#include "PrefsManager.h"
#include "RageDisplay.h"
#include "RageLog.h"
#include "ScreenDimensions.h"
#include "GameState.h"
#include "Style.h"

REGISTER_SCREEN_CLASS( ScreenStatsOverlay );

void ScreenStatsOverlay::Init()
{
	Screen::Init();

 	m_textStats.LoadFromFont( THEME->GetPathF(m_sName,"stats") );
	m_textStats.SetName( "Stats" );
	LOAD_ALL_COMMANDS_AND_SET_XY_AND_ON_COMMAND( m_textStats ); 
	this->AddChild( &m_textStats );

	/* "Was that a skip?"  This displays a message when an update takes
	 * abnormally long, to quantify skips more precisely, verify them
	 * when they're subtle, and show the time it happened, so you can pinpoint
	 * the time in the log. */
	m_LastSkip = 0;

	SHOW_SKIPS.Load( m_sName, "ShowSkips" );
	if( SHOW_SKIPS )
	{
		SKIP_X.Load( m_sName, "SkipX" );
		SKIP_Y.Load( m_sName, "SkipY" );
		SKIP_SPACING_Y.Load( m_sName, "SkipSpacingY" );
		SKIP_WIDTH.Load( m_sName, "SkipWidth" );

		Rage::RectF rectSkips = Rage::RectF(
			SKIP_X-SKIP_WIDTH/2, 
			SKIP_Y-(SKIP_SPACING_Y*NUM_SKIPS_TO_SHOW)/2 - 10, 
			SKIP_X+SKIP_WIDTH/2, 
			SKIP_Y+(SKIP_SPACING_Y*NUM_SKIPS_TO_SHOW)/2 + 10
			);
		m_quadSkipBackground.StretchTo( rectSkips );
		m_quadSkipBackground.SetDiffuse( Rage::Color(0,0,0,0.4f) );
		this->AddChild(&m_quadSkipBackground);

		for( int i=0; i<NUM_SKIPS_TO_SHOW; i++ )
		{
			/* This is somewhat big.  Let's put it on the right side, where it'll
			 * obscure the 2P side during gameplay; there's nowhere to put it that
			 * doesn't obscure something, and it's just a diagnostic. */
			m_textSkips[i].LoadFromFont( THEME->GetPathF("Common","normal") );
			m_textSkips[i].SetX( SKIP_X );
			m_textSkips[i].SetY( 
				Rage::scale( i + 0.f, 0.f, NUM_SKIPS_TO_SHOW-1.f, rectSkips.top + 10, rectSkips.bottom - 10 )
				);
			m_textSkips[i].SetDiffuse( Rage::Color(1,1,1,0) );
			m_textSkips[i].SetShadowLength( 0 );
			this->AddChild( &m_textSkips[i] );
		}
	}

	this->SubscribeToMessage( "ShowStatsChanged" );
}

void ScreenStatsOverlay::Update( float fDeltaTime )
{
	Screen::Update(fDeltaTime);

	static bool bShowStatsWasOn = false;

	if( PREFSMAN->m_bShowStats && !bShowStatsWasOn )
	{
		// Reset skip timer when we toggle Stats on so we don't show a large skip
		// from the span when stats were turned off.
		m_timerSkip.Touch();
	}
	bShowStatsWasOn = PREFSMAN->m_bShowStats.Get();

	this->SetVisible( PREFSMAN->m_bShowStats );
	if( PREFSMAN->m_bShowStats )
	{
		std::string debugText = DISPLAY->GetStats();
		//const Style* pStyle = GAMESTATE->GetCurrentStyle(PLAYER_4);
		//if (pStyle) {
		//	debugText.append("\n");
		//	debugText.append(StyleTypeToString(pStyle->m_StyleType));
		//}

		m_textStats.SetText( debugText );
		if ( SHOW_SKIPS )
			UpdateSkips();
	}
}

void ScreenStatsOverlay::AddTimestampLine( const std::string &txt, const Rage::Color &color )
{
	m_textSkips[m_LastSkip].SetText( txt );
	m_textSkips[m_LastSkip].StopTweening();
	m_textSkips[m_LastSkip].SetDiffuse( Rage::Color(1,1,1,1) );
	m_textSkips[m_LastSkip].BeginTweening( 0.2f );
	m_textSkips[m_LastSkip].SetDiffuse( color );
	m_textSkips[m_LastSkip].BeginTweening( 3.0f );
	m_textSkips[m_LastSkip].BeginTweening( 0.2f );
	m_textSkips[m_LastSkip].SetDiffuse( Rage::Color(1,1,1,0) );

	m_LastSkip++;
	m_LastSkip %= NUM_SKIPS_TO_SHOW;
}

void ScreenStatsOverlay::UpdateSkips()
{
	/* Use our own timer, so we ignore `/tab. */
	const float UpdateTime = m_timerSkip.GetDeltaTime();

	/* FPS is 0 for a little while after we load a screen; don't report
	 * during this time. Do clear the timer, though, so we don't report
	 * a big "skip" after this period passes. */
	if( !DISPLAY->GetFPS() )
		return;

	/* We want to display skips.  We expect to get updates of about 1.0/FPS ms. */
	const float ExpectedUpdate = 1.0f / DISPLAY->GetFPS();

	/* These are thresholds for severity of skips.  The smallest
	 * is slightly above expected, to tolerate normal jitter. */
	const float Thresholds[] =
	{
		ExpectedUpdate * 2.0f, ExpectedUpdate * 4.0f, 0.1f, -1
	};

	int skip = 0;
	while( Thresholds[skip] != -1 && UpdateTime > Thresholds[skip] )
		skip++;

	if( skip )
	{
		std::string sTime( SecondsToMMSSMsMs(RageTimer::GetTimeSinceStartFast()) );

		static const Rage::Color colors[] =
		{
			Rage::Color(0,0,0,0),			/* unused */
			Rage::Color(1.0f,1.0f,1.0f,1),	/* white*/
			Rage::Color(1.0f,1.0f,0.0f,1),	/* yellow */
			Rage::Color(1.0f,0.4f,0.4f,1)		/* light red */
		};

		AddTimestampLine( fmt::sprintf("%s: %.0fms (%.0f)", sTime.c_str(), 1000*UpdateTime, UpdateTime/ExpectedUpdate), colors[skip] );

		if( PREFSMAN->m_bLogSkips )
			LOG->Trace( "Frame skip: %.0fms (%.0f)", 1000*UpdateTime, UpdateTime/ExpectedUpdate );
	}
}

/*
 * (c) 2001-2005 Chris Danford, Glenn Maynard
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

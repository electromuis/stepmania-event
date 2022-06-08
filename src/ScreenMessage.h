#ifndef ScreenMessage_H
#define ScreenMessage_H
/** @brief Definition of common ScreenMessages and helpers. */
typedef RString ScreenMessage;

GLOBALS_IMPORT_PREFIX extern const ScreenMessage SM_Invalid;
GLOBALS_IMPORT_PREFIX extern const ScreenMessage SM_None;
GLOBALS_IMPORT_PREFIX extern const ScreenMessage SM_MenuTimer;
GLOBALS_IMPORT_PREFIX extern const ScreenMessage SM_DoneFadingIn;
GLOBALS_IMPORT_PREFIX extern const ScreenMessage SM_BeginFadingOut;
GLOBALS_IMPORT_PREFIX extern const ScreenMessage SM_GoToNextScreen;
GLOBALS_IMPORT_PREFIX extern const ScreenMessage SM_GoToPrevScreen;
GLOBALS_IMPORT_PREFIX extern const ScreenMessage SM_GainFocus;
GLOBALS_IMPORT_PREFIX extern const ScreenMessage SM_LoseFocus;
GLOBALS_IMPORT_PREFIX extern const ScreenMessage SM_Pause;
GLOBALS_IMPORT_PREFIX extern const ScreenMessage SM_Success;
GLOBALS_IMPORT_PREFIX extern const ScreenMessage SM_Failure;
/** @brief Helpers for the ScreenMessages. */
namespace ScreenMessageHelpers
{
	ScreenMessage ToScreenMessage( const RString & Name );
	RString	ScreenMessageToString( ScreenMessage SM );
};

/** @brief Automatically generate a unique ScreenMessage value */
#define AutoScreenMessage( x ) \
	const ScreenMessage x = ScreenMessageHelpers::ToScreenMessage( #x )

#endif

/**
 * @file
 * @author Chris Danford, Charles Lohr (c) 2003-2005
 * @section LICENSE
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

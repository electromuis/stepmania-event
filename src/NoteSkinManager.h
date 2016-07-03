#ifndef NEW_SKIN_MANAGER_H
#define NEW_SKIN_MANAGER_H

#include "NoteSkin.h"

struct NoteSkinManager
{
	NoteSkinManager();
	~NoteSkinManager();

	void load_skins();
	void get_skins_for_stepstype(StepsType type, std::vector<NoteSkinLoader const*>& ret);
	void get_all_skin_names(std::vector<std::string>& ret);
	void get_skin_names_for_stepstype(StepsType type, std::vector<std::string>& ret);
	std::string get_first_skin_name_for_stepstype(StepsType type);
	std::vector<StepsType> const& get_supported_stepstypes();
	bool skin_supports_stepstype(std::string const& skin, StepsType type);
	NoteSkinLoader const* get_loader_for_skin(std::string const& skin_name);
	std::string get_path(NoteSkinLoader const* skin,
		std::string file);
	bool named_skin_exists(std::string const& skin_name);

	void PushSelf(lua_State* L);

private:
	std::vector<NoteSkinLoader> m_skins;
	std::vector<StepsType> m_supported_types;
};

extern NoteSkinManager* NOTESKIN; // global and accessible from anywhere in our program

#endif

/**
 * @file
 * @author Chris Danford (c) 2003-2004
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
 * 
 * (c) 2016- Electromuis, Anton Grootes
 * This branch of https://github.com/stepmania/stepmania
 * will from here on out be released as GPL v3 (wich converts from the previous MIT license)
 */

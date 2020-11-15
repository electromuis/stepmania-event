#include "global.h"

#include "arch/ArchHooks/ArchHooks.h"
#include "LuaManager.h"
#include "RageFileManager.h"

#include "NotesLoaderSM.h"
#include "NotesWriterPadmiss.h"
#include "Song.h"

#include <iostream>

using namespace std;

#define SAFE_DELETE(p)       do { delete (p);     (p)=nullptr; } while( false )

int main(int argc, char* argv[]) {
	//return sm_main(argc, argv);

	HOOKS = ArchHooks::Create();
	HOOKS->Init();

	LUA = new LuaManager;
	HOOKS->RegisterWithLua();

	FILEMAN = new RageFileManager(argv[0]);
	FILEMAN->MountInitialFilesystems();

	RString sPath = "/";

	vector<RString> files;
	SMLoader loaderSM;

	loaderSM.GetApplicableFiles(sPath, files);

	for (auto f : files) {
		Song* pSong = new Song;

		if(loaderSM.LoadFromSimfile(f, *pSong, false)) {
			std::cout << "Loaded: " << f << "\n";
		}
		else {
			std::cout << "Failed loading: " << f << "\n";
			continue;
		}

		int chartNum = 0;
		for (auto step : pSong->GetAllSteps()) {
			RString outputFile = f + "-" + std::to_string(chartNum) + ".json";

			if (!FILEMAN->DoesFileExist(outputFile)) {
				NotesWriterPadmiss::WriteSteps(outputFile, *step);

				std::cout << "Wrote json for: " << outputFile << "\n";
			}
			else {
				std::cout << outputFile << " already exists\n";
			}

			chartNum++;
		}

		SAFE_DELETE(pSong);
	}

	SAFE_DELETE(FILEMAN);
	SAFE_DELETE(LUA);
	SAFE_DELETE(PREFSMAN);
}

/*
 * (c) 2014 Jason Felds
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

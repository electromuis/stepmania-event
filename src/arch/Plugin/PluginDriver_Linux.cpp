#include "global.h"
#include "RageFileManager.h"
#include "PluginDriver_Linux.h"
#include "RageLog.h"

#include <iostream>

LoadedPluginLinux::LoadedPluginLinux(RString libraryPath)
	:LoadedPlugin(libraryPath)
{

}

void PluginDriver_Linux::GetAvailablePlugins(std::vector<LoadedPlugin*>& out)
{
	PluginDriver::GetAvailablePlugins(out);
	
	vector<RString> files = vector<RString>();
	FILEMAN->GetDirListing("Plugins\\*.so", files, false, true);

	for (RString file : files)
	{
		file = FILEMAN->ResolvePath(file);

		try {
			LoadedPlugin* lp = new LoadedPluginLinux(file);
			out.push_back(lp);
		}
		catch (exception e) {
			cout << "E1" << e.what() << endl;
			LOG->Trace("Failed loading plugin (Scan): %s, exception: %s", file.c_str(), e.what());
		}
		catch (string e) {
			cout << "E2" << e << endl;
			LOG->Trace("Failed loading plugin (Scan): %s, exception: %s", file.c_str(), e.c_str());
		}
		catch (...) {
			cout << "E3" << endl;
			LOG->Trace("Failed loading plugin (Scan): %s", file.c_str());
		}
	}
}

#include "RageFileManager.h"
#include "PluginDriver_Linux.h"
#include "RageLog.h"

LoadedPluginLinux::LoadedPluginLinux(RString libraryPath)
	:LoadedPlugin(libraryPath)
{

}

void LoadedPluginLinux::GetAvailablePlugins(std::vector<LoadedPlugin*>& out)
{
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
			LOG->Info("Failed loading plugin file: %s, exception: %s", file.c_str(), e.what());
		}
	}
}

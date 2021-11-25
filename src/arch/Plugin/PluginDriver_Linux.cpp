#include "RageFileManager.h"
#include "PluginDriver_Linux.h"
#include "RageLog.h"

LoadedPluginLinux::LoadedPluginLinux(RString libraryPath)
	:LoadedPlugin(libraryPath)
{

}

bool LoadedPluginLinux::Load()
{
	if (loaded)
		return false;

	loadedLibrary = new dynalo::library(libraryPath);

	if (loadedLibrary->get_native_handle() == dynalo::native::invalid_handle())
		throw "Lib invalid";

	loadedDetails = loadedLibrary->get_function<PluginDetails>("exports");
	pluginBase = loadedDetails->initializeFunc(libraryPath);
	pluginName = loadedDetails->pluginName;

	loaded = true;
	return loaded;
}

bool LoadedPluginLinux::Unload()
{
	if (!loaded)
		return false;

	//delete(loadedDetails);
	//delete(pluginBase);
	delete(loadedLibrary);

	loaded = false;
	return true;
}

bool LoadedPluginLinux::IsLoaded()
{
	return loaded;
}

PluginBase* LoadedPluginLinux::GetPlugin()
{
	if (!loaded)
		return nullptr;

	return pluginBase;
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

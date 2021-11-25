#include "global.h"
#include "RageFileManager.h"
#include "PluginDriver_Win32.h"
#include "RageLog.h"

LoadedPluginWin32::LoadedPluginWin32(RString libraryPath)
	:LoadedPlugin(libraryPath)
{

}

bool LoadedPluginWin32::Load()
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

bool LoadedPluginWin32::Unload()
{
	if (!loaded)
		return false;

	//delete(loadedDetails);
	//delete(pluginBase);
	delete(loadedLibrary);

	loaded = false;
	return true;
}

bool LoadedPluginWin32::IsLoaded()
{
	return loaded;
}

PluginBase* LoadedPluginWin32::GetPlugin()
{
	if (!loaded)
		return nullptr;

	return pluginBase;
}

void PluginDriver_Win32::GetAvailablePlugins(std::vector<LoadedPlugin*>& out)
{
	vector<RString> files = vector<RString>();
	FILEMAN->GetDirListing("Plugins\\*.dll", files, false, true);

	for (RString file : files)
	{
		file = FILEMAN->ResolvePath(file);
		file = file.substr(1);

		try {
			LoadedPlugin* lp = new LoadedPluginWin32(file);
			out.push_back(lp);
		}
		catch (exception e) {
			LOG->Info("Failed loading plugin file: %s, exception: %s", file.c_str(), e.what());
		}
	}
}

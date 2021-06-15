#include "global.h"

#include "dynalo/dynalo.hpp"

#include <PluginManager.h>
#include <RageFileManager.h>
#include <RageLog.h>



PluginManager* PLUGINMAN = nullptr;


LoadedPlugin::LoadedPlugin(RString libraryPath)
{
	loaded = false;

	loadedLibrary = new dynalo::library(libraryPath);

	if (loadedLibrary->get_native_handle() == dynalo::native::invalid_handle()) {
		throw "Failed loading: " + libraryPath;
	}

	loadedDetails = loadedLibrary->get_function<PluginDetails>("exports");
	
	LOG->Info("Plugin name: " + RString(loadedDetails->pluginName));
	Load();
}

bool LoadedPlugin::Load()
{
	pluginBase = loadedDetails->initializeFunc();

	return true;
}

PluginManager::PluginManager()
{	
	LoadAvailiblePlugins();
}

PluginManager::~PluginManager()
{

}

void PluginManager::LoadAvailiblePlugins()
{
	vector<RString> files = vector<RString>();

	LOG->Info("Reading plugins: ");

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#define LOAD_WIN32
#endif

#if defined(LOAD_WIN32)
	FILEMAN->GetDirListing("Plugins\\*.dll", files, false, true);
#else
	FILEMAN->GetDirListing("Plugins\\*.so", files, false, true);
#endif

	LOG->Info("Count: " + files.size());
	for (RString file : files)
	{
		file = FILEMAN->ResolvePath(file);

		// Remove leading / on windows
#if defined(LOAD_WIN32)
		file = file.substr(1);
#endif

		LOG->Info("Attempting to load plugin: " + file);

		try {
			plugins.push_back(LoadedPlugin(file));
		}
		catch (...) {
			LOG->Info("Failed loading: " + file);
		}
	}
}

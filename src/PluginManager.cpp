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
	LoadStepmaniaPointer();
	LoadAvailiblePlugins();
}

void PluginManager::LoadStepmaniaPointer()
{
	stepmaniaPointer.hooks = HOOKS;
	stepmaniaPointer.lua = LUA;
	stepmaniaPointer.log = LOG;
	stepmaniaPointer.prefsManager = PREFSMAN;
	stepmaniaPointer.messageManager = MESSAGEMAN;
	stepmaniaPointer.gameState = GAMESTATE;
	stepmaniaPointer.gameManager = GAMEMAN;
	stepmaniaPointer.songManager = SONGMAN;
	stepmaniaPointer.bookKeeper = BOOKKEEPER;
}

PluginManager::~PluginManager()
{
	for (LoadedPlugin* plugin : plugins)
	{
		delete plugin;
	}

	plugins.clear();
}

void PluginManager::LoadAvailiblePlugins()
{
	vector<LoadedPlugin*> plugins = vector<LoadedPlugin*>();
	vector<RString> files = vector<RString>();

	LOG->Info("Reading plugins: ");

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	FILEMAN->GetDirListing("Plugins\\*.dll", files, false, true);
#else
	FILEMAN->GetDirListing("Plugins\\*.so", files, false, true);
#endif

	LOG->Info("Count: " + files.size());
	for (RString file : files)
	{
		file = RageFileManagerUtil::sInitialWorkingDirectory + file;

		LOG->Info("Attempting to load plugin: " + file);

		LoadedPlugin* plugin = nullptr;

		try {
			plugin = new LoadedPlugin(file);
		}
		catch (...) {
			LOG->Info("Failed loading: " + file);
		}
	
		plugins.push_back(plugin);
	}
}

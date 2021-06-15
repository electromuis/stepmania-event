#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include "global.h"

#define DYNALO_DEMANGLE
#define DYNALO_EXPORT_SYMBOLS
#include "dynalo/symbol_helper.hpp"
#include "dynalo/dynalo.hpp"
using namespace dynalo;

// All the includes we need in out StepmaniaPointer

#include "AnnouncerManager.h"
#include "arch/ArchHooks/ArchHooks.h"
#include "LuaManager.h"
#include "RageLog.h"
#include "PrefsManager.h"
#include "MessageManager.h"
#include "GameState.h"
#include "GameManager.h"
#include "SongManager.h"
#include "Bookkeeper.h"

class PluginManager;

class DYNALO_EXPORT PluginBase;
DYNALO_EXPORT typedef PluginBase* (*GetPluginFunc)();

struct PluginDetails
{
	int apiVersion;
	const char* fileName;
	const char* className;
	const char* pluginName;
	const char* pluginVersion;
	GetPluginFunc initializeFunc;
};

#define PLUGIN_API_VERSION 1
#define STANDARD_PLUGIN_STUFF PLUGIN_API_VERSION, __FILE__

#define REGISTER_PLUGIN(classType, pluginName, pluginVersion)                  \
extern "C" {																   \
    DYNALO_EXPORT PluginBase* getPlugin()									   \
    {                                                                          \
        static classType singleton;                                            \
        return &singleton;                                                     \
    }                                                                          \
    DYNALO_EXPORT PluginDetails exports = {								       \
        STANDARD_PLUGIN_STUFF,                                                 \
        #classType,                                                            \
        pluginName,                                                            \
        pluginVersion,                                                         \
        getPlugin,                                                             \
    };                                                                         \
}

class PluginBase {
public:
	PluginBase() {};

	bool Update(float fDeltaTime) { return false; };
};

class LoadedPlugin  {
public:
	LoadedPlugin(RString libraryPath);
	
	bool Load();

	bool IsLoaded()
	{
		return loaded;
	}

private:
	bool loaded = false;
	library* loadedLibrary = nullptr;
	PluginDetails* loadedDetails = nullptr;
	PluginBase* pluginBase = nullptr;
};

class PluginManager
{
public:
	PluginManager();
	~PluginManager();
private:
	void LoadAvailiblePlugins();

	vector<LoadedPlugin> plugins;
};

extern PluginManager* PLUGINMAN;

#endif

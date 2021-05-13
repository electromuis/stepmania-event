#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include "global.h"

#define DYNALO_DEMANGLE
#define DYNALO_EXPORT_SYMBOLS
#include "dynalo/symbol_helper.hpp"
#include "dynalo/dynalo.hpp"
using namespace dynalo;

class DYNALO_EXPORT IPlugin;
DYNALO_EXPORT typedef IPlugin* (*GetPluginFunc)();

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
    DYNALO_EXPORT IPlugin* getPlugin()									       \
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
	PluginBase();
	virtual ~PluginBase();

	void Update(float fDeltaTime);
};

class LoadedPlugin  {
public:
	LoadedPlugin(RString libraryPath);
	~LoadedPlugin();
	bool Load();

	void Update(float fDeltaTime);

	bool IsLoaded()
	{
		return loaded;
	}

private:
	bool loaded = false;
	library library;
	PluginDetails* loadedDetails = nullptr;
};

class PluginManager
{
public:
	PluginManager();
	~PluginManager();

	void loadAvailiblePlugins();
private:
	vector<LoadedPlugin*> plugins;
};

extern PluginManager* PLUGINMAN;

#endif

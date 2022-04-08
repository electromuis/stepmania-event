#ifndef PluginDriver_H
#define PluginDriver_H

#include <vector>
#include <memory>

#define DYNALO_DEMANGLE
#define DYNALO_EXPORT_SYMBOLS
#include "dynalo/symbol_helper.hpp"
#include "dynalo/dynalo.hpp"
using namespace dynalo;

#include "LoadedPlugin.h"

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

#ifdef WITH_PLUGINS_EMBEDDED
#define REGISTER_DETAILS(classType, pluginVersion)						\
DYNALO_EXPORT extern PluginDetails classType##_Details = {				\
	PLUGIN_API_VERSION,													\
	__FILE__,															\
	#classType,                                                         \
	PLUGIN_NAME,                                                        \
	pluginVersion,                                                      \
	getPlugin_##classType												\
};
#else
#define REGISTER_DETAILS(classType, pluginVersion)						\
DYNALO_EXPORT PluginDetails exports = {									\
	PLUGIN_API_VERSION,													\
	__FILE__,															\
	#classType,                                                         \
	PLUGIN_NAME,                                                        \
	pluginVersion,                                                      \
	getPlugin_##classType												\
};     
#endif

#define REGISTER_PLUGIN(classType, pluginVersion)											\
extern "C" {																				\
    DYNALO_EXPORT PluginBase* getPlugin_##classType()										\
    {																						\
        return new classType();																\
    }																						\
    REGISTER_DETAILS(classType, pluginVersion)												\
}

class PluginDriver
{
public:
	static PluginDriver* Create();
	virtual void GetAvailablePlugins(std::vector<LoadedPlugin*>& out) = 0;
};

#endif

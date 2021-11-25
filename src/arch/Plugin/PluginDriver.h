#ifndef PluginDriver_H
#define PluginDriver_H

#include <vector>

#define DYNALO_DEMANGLE
#define DYNALO_EXPORT_SYMBOLS
#include "dynalo/symbol_helper.hpp"
#include "dynalo/dynalo.hpp"
using namespace dynalo;

#include "global.h"
#include "LuaManager.h"

class LoadedPlugin;
class DYNALO_EXPORT PluginBase;
DYNALO_EXPORT typedef PluginBase* (*GetPluginFunc)(std::string libraryPath);

struct PluginDetails
{
	int apiVersion;
	const char* fileName;
	const char* className;
	const char* pluginName;
	const char* pluginVersion;
	GetPluginFunc initializeFunc;
};

typedef int (plugin_binding_t)(LoadedPlugin* p, lua_State* L);

struct PluginRegType
{
	const char* szName;
	plugin_binding_t* mfunc;
};

class PluginBase {
public:
	PluginBase() {};

	virtual void Update(float fDeltaTime) = 0;
	virtual void PluginFree(void* p) { free(p); };
	virtual void PluginDelete(void* p) { delete(p); };
	virtual bool HasScreen(const char* sName) { return false; };
	virtual std::vector<PluginRegType>* GetLuaFunctions() { return nullptr; };
};

#define PLUGIN_API_VERSION 1
#define STANDARD_PLUGIN_STUFF PLUGIN_API_VERSION, __FILE__

#define REGISTER_PLUGIN(classType, pluginName, pluginVersion)                  \
extern "C" {																   \
    DYNALO_EXPORT PluginBase* getPlugin(std::string libraryPath)			   \
    {                                                                          \
        static classType singleton(libraryPath);                               \
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
class LoadedPlugin {
public:
	LoadedPlugin(RString libraryPath);
	virtual ~LoadedPlugin();

	virtual bool Load() = 0;
	virtual bool Unload();
	virtual bool IsLoaded() = 0;
	virtual PluginBase* GetPlugin() = 0;

	void Update(float fDeltaTime);
	void PluginFree(void* ptr);
	void PluginDelete(void* ptr);
	bool HasScreen(const char* sName);

	RString GetLibraryPath()
	{
		return libraryPath;
	}

	RString GetName()
	{
		return pluginName;
	}

	// Lua
	void PushSelf(lua_State* L);

protected:
	RString libraryPath;
	RString pluginName;
};

class PluginDriver
{
public:
	static PluginDriver* Create();
	virtual void GetAvailablePlugins(std::vector<LoadedPlugin*>& out) = 0;
};

#endif

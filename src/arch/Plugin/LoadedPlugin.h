#pragma once

#define DYNALO_DEMANGLE
#define DYNALO_EXPORT_SYMBOLS
#include "dynalo/symbol_helper.hpp"
#include "dynalo/dynalo.hpp"
using namespace dynalo;

#include "PluginBase.h"

enum PluginLoadPhase
{
	PluginLoadPhase_Library,
	PluginLoadPhase_Plugin
};

class PluginDetails;
struct lua_State;

class LoadedPlugin {
public:
	LoadedPlugin(std::string libraryPath);
	virtual ~LoadedPlugin();

	virtual bool Load(PluginLoadPhase phase = PluginLoadPhase_Plugin);
	virtual bool Unload();
	virtual bool IsLoaded(PluginLoadPhase phase = PluginLoadPhase_Plugin);
	virtual PluginBase* GetPlugin();

	void Update(float fDeltaTime);
	void PluginFree(void* ptr);
	void PluginDelete(void* ptr);
	bool HasScreen(const char* sName);

	std::string GetLibraryPath()
	{
		return libraryPath;
	}

	std::string GetName()
	{
		return pluginName;
	}

	// Lua
	void PushSelf(lua_State* L);

protected:
	library* loadedLibrary = nullptr;
	PluginDetails* loadedDetails = nullptr;
	PluginBase* pluginBase = nullptr;

	std::string libraryPath;
	std::string pluginName;
};

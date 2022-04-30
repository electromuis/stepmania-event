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
	virtual ~LoadedPlugin();

	virtual bool Load(PluginLoadPhase phase = PluginLoadPhase_Plugin) { return false; };
	virtual bool Unload() { return false; };
	virtual bool IsLoaded(PluginLoadPhase phase = PluginLoadPhase_Plugin) = 0;
	virtual PluginBase* GetPlugin();

	template <typename FunctionSignature>
	inline
	FunctionSignature GetSymbol(const char* name)
	{
		if (!Load(PluginLoadPhase_Plugin))
			return nullptr;

		auto p = GetPlugin();
		if (!p)
			return nullptr;

		void* sym = p->GetSymbol(name);
		if (sym == nullptr)
			return nullptr;

		return reinterpret_cast<FunctionSignature>(sym);
	};

	void Update(float fDeltaTime);
	void PluginFree(void* ptr);

	std::string GetName()
	{
		return pluginName;
	}

	// Lua
	void PushSelf(lua_State* L);

protected:
	LoadedPlugin();
	std::string pluginName;
	PluginBase* pluginBase = nullptr;
};

class LoadedPluginLibrary : public LoadedPlugin {
public:
	LoadedPluginLibrary(std::string libraryPath);

	virtual bool Load(PluginLoadPhase phase);
	virtual bool Unload();
	virtual bool IsLoaded(PluginLoadPhase phase);

	std::string GetLibraryPath()
	{
		return libraryPath;
	}

protected:
	library* loadedLibrary = nullptr;
	PluginDetails* loadedDetails = nullptr;

	std::string libraryPath;
};

class LoadedPluginEmbedded : public LoadedPlugin {
public:
	LoadedPluginEmbedded(PluginDetails& details);

	virtual bool Load(PluginLoadPhase phase);
	virtual bool Unload();
	virtual bool IsLoaded(PluginLoadPhase phase);
	static std::vector<LoadedPluginEmbedded*> GetList();
protected:
	PluginDetails& loadedDetails;
	static std::vector<LoadedPluginEmbedded*> list;
};

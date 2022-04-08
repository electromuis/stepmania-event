#include "global.h"
#include "LoadedPlugin.h"
#include "PluginBase.h"
#include "PluginDriver.h"

// LoadedPlugin

LoadedPlugin::LoadedPlugin()
{
	Load(PluginLoadPhase_Library);
	Unload();
}

PluginBase* LoadedPlugin::GetPlugin()
{
	if (!IsLoaded(PluginLoadPhase_Plugin))
		return nullptr;

	return pluginBase;
}

LoadedPlugin::~LoadedPlugin()
{
	Unload();
}

void LoadedPlugin::Update(float fDeltaTime)
{
	if (!IsLoaded())
		return;

	PluginBase* p = GetPlugin();
	ASSERT_M(p, "Not really loaded");
	p->Update(fDeltaTime);
}

void LoadedPlugin::PluginFree(void* ptr)
{
	if (!IsLoaded())
		return;

	PluginBase* p = GetPlugin();
	ASSERT_M(p, "Not really loaded");
	p->PluginFree(ptr);
}

// LoadedPluginLibrary

LoadedPluginLibrary::LoadedPluginLibrary(std::string libraryPath)
	:libraryPath(libraryPath), LoadedPlugin()
{

}

bool LoadedPluginLibrary::Load(PluginLoadPhase phase)
{
	if (IsLoaded(phase))
		return true;

	if (!loadedLibrary)
		loadedLibrary = new dynalo::library(libraryPath);

	if (loadedLibrary->get_native_handle() == dynalo::native::invalid_handle())
		throw "Lib invalid";

	loadedDetails = loadedLibrary->get_function<PluginDetails>("exports");
	pluginName = loadedDetails->pluginName;

	if (phase == PluginLoadPhase_Library)
		return true;

	pluginBase = loadedDetails->initializeFunc();

	return true;
}

bool LoadedPluginLibrary::Unload()
{
	delete(pluginBase);
	delete(loadedLibrary);
	loadedDetails = nullptr;
	pluginBase = nullptr;
	loadedLibrary = nullptr;

	return true;
}

bool LoadedPluginLibrary::IsLoaded(PluginLoadPhase phase)
{
	if (phase == PluginLoadPhase_Library)
		return loadedLibrary;

	if (phase == PluginLoadPhase_Plugin)
		return pluginBase;

	return false;
}

// LoadedPluginEmbedded

std::vector<LoadedPluginEmbedded*> LoadedPluginEmbedded::list;

LoadedPluginEmbedded::LoadedPluginEmbedded(PluginDetails& details)
	:loadedDetails(details), LoadedPlugin()
{
	list.push_back(this);
}

std::vector<LoadedPluginEmbedded*> LoadedPluginEmbedded::GetList()
{
	return list;
}

bool LoadedPluginEmbedded::Load(PluginLoadPhase phase)
{
	if (IsLoaded(phase))
		return true;

	pluginName = loadedDetails.pluginName;

	if (phase == PluginLoadPhase_Library)
		return true;

	pluginBase = loadedDetails.initializeFunc();

	return true;
}

bool LoadedPluginEmbedded::Unload()
{
	delete(pluginBase);
	pluginBase = nullptr;

	return true;
}

bool LoadedPluginEmbedded::IsLoaded(PluginLoadPhase phase)
{
	if (phase == PluginLoadPhase_Library)
		return true;

	if (phase == PluginLoadPhase_Plugin)
		return pluginBase;

	return false;
}

// lua start
#include "LuaBinding.h"
#include "LuaReference.h"
#include "LuaManager.h"

class LunaLoadedPlugin : public Luna<LoadedPlugin>
{
public:
	static int GetName(T* p, lua_State* L)
	{
		lua_pushstring(L, p->GetName().c_str());
		return 1;
	}

	LunaLoadedPlugin()
	{
		ADD_METHOD(GetName);
	}
};

template<>
void Luna<LoadedPlugin>::PushObject(Lua* L, const RString& sDerivedClassName, LoadedPlugin* p)
{
	// Get the base lua definition and add the Plugin's own lua methods to that

	PluginBase* pb = p->GetPlugin();
	auto regs = pb->GetLuaFunctions();

	void** pData = (void**)lua_newuserdata(L, sizeof(void*));
	*pData = p;
	int retTable = lua_gettop(L);

	luaL_getmetatable(L, sDerivedClassName);
	int metTable = lua_gettop(L);

	lua_getfield(L, metTable, "__index");
	int funcTable = lua_gettop(L);

	if (regs) {
		for (PluginRegType& r : *regs) {
			lua_pushlightuserdata(L, (void*)r.mfunc);
			lua_pushcclosure(L, thunk, 1);
			lua_setfield(L, funcTable, r.szName);
		}

		pb->PluginDelete(regs);
	}
	
	lua_pop(L, 1);
	lua_setmetatable(L, retTable);
}

template<> RString Luna<LoadedPlugin>::m_sClassName = "LoadedPlugin";
template<> RString Luna<LoadedPlugin>::m_sBaseClassName = "LoadedPlugin";

void LoadedPlugin::PushSelf(lua_State* L)
{
	Luna<LoadedPlugin>::PushObject(L, Luna<LoadedPlugin>::m_sClassName, this);
}
static LunaLoadedPlugin registeraLoadedPlugin;

namespace LuaHelpers
{
	template<>
	void Push<LoadedPlugin*>(lua_State* L, LoadedPlugin* const& pObject)
	{
		if (pObject == nullptr)
			lua_pushnil(L);
		else
			pObject->PushSelf(L);
	}
}

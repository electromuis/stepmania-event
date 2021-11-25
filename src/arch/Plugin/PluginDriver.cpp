#include "PluginDriver.h"

LoadedPlugin::LoadedPlugin(RString libraryPath)
	:libraryPath(libraryPath)
{
	
}

bool LoadedPlugin::Unload()
{
	return false;
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

void LoadedPlugin::PluginDelete(void* ptr)
{
	if (!IsLoaded())
		return;

	PluginBase* p = GetPlugin();
	ASSERT_M(p, "Not really loaded");
	p->PluginDelete(ptr);
}

bool LoadedPlugin::HasScreen(const char* name)
{
	if (!IsLoaded())
		return false;

	PluginBase* p = GetPlugin();
	ASSERT_M(p, "Not really loaded");
	return p->HasScreen(name);
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
		lua_pushstring(L, p->GetName()); return 1;
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

#include "arch/arch_default.h"
PluginDriver* PluginDriver::Create()
{
	PluginDriver* ret = nullptr;

#ifdef ARCH_PLUGIN_DRIVER
		ret = new ARCH_PLUGIN_DRIVER;
#endif

	if (!ret)
		ret = new PluginDriver_Null();

	return ret;
}

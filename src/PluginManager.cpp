#include "global.h"
#include "PluginManager.h"
#include "RageLog.h"
#include "PrefsManager.h"
#include "ThemeManager.h"

PluginManager* PLUGINMAN = nullptr;

PluginManager::PluginManager()
{	
	m_pDriver = PluginDriver::Create();
	m_pDriver->GetAvailablePlugins(plugins);

	{
		Lua* L = LUA->Get();
		lua_pushstring(L, "PLUGINMAN");
		this->PushSelf(L);
		lua_settable(L, LUA_GLOBALSINDEX);
		LUA->Release(L);
	}
}

PluginManager::~PluginManager()
{
	UnloadAll();

	//for (LoadedPlugin* p : plugins)
		//delete(p);

	delete(m_pDriver);
}

void PluginManager::LoadAll()
{
	LOG->Trace("PluginManager :: loading plugins");

	for (LoadedPlugin* p : plugins)
	{
		try {
			p->Load(PluginLoadPhase_Library);
			bool f = true;
		}
		catch (exception e) {
			LOG->Trace("Failed loading plugin (Lib) exception: %s", e.what());
		}
		catch (string e) {
			LOG->Trace("Failed loading plugin (Lib) exception: %s", e.c_str());
		}
		catch (...) {
			LOG->Trace("Failed loading plugin (Lib)");
		}
	}

	// Sepperate phases to fix global preference variables not loading

	PREFSMAN->ReadPrefsFromDisk();

	for (LoadedPlugin* p : plugins)
	{
		try {
			p->Load(PluginLoadPhase_Plugin);
			bool f = true;
		}
		catch (exception e) {
			LOG->Info("Failed loading plugin (Plugin) exception: %s", e.what());
		}
		catch (string e) {
			LOG->Info("Failed loading plugin (Plugin) exception: %s", e.c_str());
		}
		catch (...) {
			LOG->Info("Failed loading plugin (Plugin)");
		}
	}

	THEME->ReloadMetrics();
}

void PluginManager::UnloadAll()
{
	for (LoadedPlugin* p : plugins)
	{
		try {
			p->Unload();
		}
		catch (exception e) {
			LOG->Info("Failed unloading plugin exception: %s", e.what());
		}
		catch (...) {
			LOG->Info("Failed unloading plugin");
		}
	}
}

void PluginManager::Update(float fDeltaTime)
{
	for (LoadedPlugin* plugin : plugins)
	{
		plugin->Update(fDeltaTime);
	}
}

void PluginManager::AppFree(void* addr)
{
	free(addr);
}

void PluginManager::AppDelete(void* addr)
{
	delete(addr);
}

void* PluginManager::AppAllocate(size_t size)
{
	return malloc(size);
}

LoadedPlugin* PluginManager::GetPlugin(int index)
{
	if (index >= 0 && index < (plugins.size() - 1))
		return plugins.at(index);

	return nullptr;
}

LoadedPlugin* PluginManager::GetPlugin(RString name)
{
	std::vector<LoadedPlugin*>::iterator it = std::find_if(plugins.begin(), plugins.end(), [name](LoadedPlugin* p) {
		return name == p->GetName();
	});

	if(it != plugins.end())
		return *it;

	return nullptr;
}

// lua start
#include "LuaBinding.h"
#include "LuaReference.h"
#include "LuaManager.h"

/** @brief Allow Lua to have access to the PluginManager. */
class LunaPluginManager : public Luna<PluginManager>
{
public:
	DEFINE_METHOD( GetNumPlugins, GetNumPlugins() )

	static int GetPlugin(T* p, lua_State* L)
	{
		LoadedPlugin* l = p->GetPlugin(SArg(1));
		if (l)
			l->PushSelf(L);
		else
			lua_pushnil(L);
		return 1;
	}

	static int GetPluginNames(T* p, lua_State* L)
	{
		vector<RString> pluginNames;

		if (p->GetNumPlugins() > 0)
		{
			for (int i = 0; i < p->GetNumPlugins(); i++)
			{
				LoadedPlugin* l = p->GetPlugin(i);
				if (!l)
					continue;

				pluginNames.push_back(l->GetName());
			}
		}

		LuaHelpers::CreateTableFromArray(pluginNames, L);

		return 1;
	}

	LunaPluginManager()
	{
		ADD_METHOD(GetNumPlugins);
		ADD_METHOD(GetPlugin);
		ADD_METHOD(GetPluginNames);
	}
};

LUA_REGISTER_CLASS(PluginManager)

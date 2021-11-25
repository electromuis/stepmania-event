#include "global.h"
#include "PluginManager.h"
#include "RageLog.h"

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

	LoadAll();
}

PluginManager::~PluginManager()
{
	UnloadAll();

	for (LoadedPlugin* p : plugins)
		delete(p);

	delete(m_pDriver);
}

void PluginManager::LoadAll()
{
	for (LoadedPlugin* p : plugins)
	{
		try {
			p->Load();
		}
		catch (exception e) {
			LOG->Info("Failed loading file: %s, exception: %s", p->GetLibraryPath().c_str(), e.what());
		}
	}
}

void PluginManager::UnloadAll()
{
	for (LoadedPlugin* p : plugins)
	{
		try {
			p->Unload();
		}
		catch (exception e) {
			LOG->Info("Failed unloading file: %s, exception: %s", p->GetLibraryPath().c_str(), e.what());
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

bool PluginManager::DeleteScreen(Screen* screen)
{
	for (LoadedPlugin* p : plugins)
	{
		if (p->HasScreen(screen->GetName().c_str())) {
			p->PluginDelete(screen);
			return true;
		}
	}

	return false;
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

LoadedPlugin* PluginManager::GetPlugin(RString name)
{
	std::vector<LoadedPlugin*>::iterator it = std::find_if(plugins.begin(), plugins.end(), [name](LoadedPlugin* p) {
		return name == p->GetName();
	});

	return *it;
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

	LunaPluginManager()
	{
		ADD_METHOD(GetNumPlugins);
		ADD_METHOD(GetPlugin);
	}
};

LUA_REGISTER_CLASS(PluginManager)

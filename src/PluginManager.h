#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include "global.h"
#include "arch/Plugin/PluginDriver.h"

#define APP_PTR(type, ...) new(PLUGINMAN->AppAllocate(sizeof(type))) type(##__VA_ARGS__)

#define DEFINE_SCREEN_CLASS( className ) \
	static Screen* Create##className( const RString &sName ) { LuaThreadVariable var( "LoadingScreen", sName ); Screen *pRet = new className; pRet->SetName( sName ); Screen::InitScreen( pRet ); return pRet; } \
	static std::unique_ptr<RegisterScreenClass> register_##className;

#define LOAD_SCREEN_CLASS( className ) \
	register_##className = std::make_unique<RegisterScreenClass>(#className, Create##className);

class PluginManager
{
public:
	PluginManager();
	~PluginManager();

	void UnloadAll();
	void LoadAll();

	void Update(float fDeltaTime);

	void* AppAllocate(size_t space);

	void AppFree(void* addr);
	void AppDelete(void* addr);
	void PluginFree(void* addr);
	void PluginDelete(void* addr);

	int GetNumPlugins() { return plugins.size(); }
	LoadedPlugin* GetPlugin(int index);
	LoadedPlugin* GetPlugin(RString name);

	// Lua
	void PushSelf(lua_State* L);

private:
	PluginDriver* m_pDriver = nullptr;
	vector<LoadedPlugin*> plugins;
};

GLOBALS_IMPORT_PREFIX extern PluginManager* PLUGINMAN;

#endif

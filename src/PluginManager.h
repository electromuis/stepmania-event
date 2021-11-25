#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include "global.h"
#include "Screen.h"
#include "arch/Plugin/PluginDriver.h"

#define APP_PTR(type, ...) new(PLUGINMAN->AppAllocate(sizeof(type))) type(##__VA_ARGS__)

class PluginManager
{
public:
	PluginManager();
	~PluginManager();

	void UnloadAll();
	void LoadAll();

	void Update(float fDeltaTime);

	bool DeleteScreen(Screen* screen);
	void* AppAllocate(size_t space);

	void AppFree(void* addr);
	void AppDelete(void* addr);
	void PluginFree(void* addr);
	void PluginDelete(void* addr);

	int GetNumPlugins() { return plugins.size(); }
	LoadedPlugin* GetPlugin(RString name);

	// Lua
	void PushSelf(lua_State* L);

private:
	PluginDriver* m_pDriver = nullptr;
	vector<LoadedPlugin*> plugins;
};

GLOBALS_IMPORT_PREFIX extern PluginManager* PLUGINMAN;

#endif

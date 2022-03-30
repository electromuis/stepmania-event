#pragma once

class LoadedPlugin;
struct lua_State;

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

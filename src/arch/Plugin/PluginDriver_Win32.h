#ifndef PluginDriver_Win32_H
#define PluginDriver_Win32_H

#include "PluginDriver.h"

class LoadedPluginWin32 : public LoadedPlugin {
public:
	LoadedPluginWin32(RString libraryPath);

	virtual bool Load();
	virtual bool Unload();
	virtual bool IsLoaded();
	virtual PluginBase* GetPlugin();
private:
	bool loaded = false;
	library* loadedLibrary = nullptr;
	PluginDetails* loadedDetails = nullptr;
	PluginBase* pluginBase = nullptr;
};

class PluginDriver_Win32 : public PluginDriver
{
public:
	virtual void GetAvailablePlugins(std::vector<LoadedPlugin*>& out);
};

#ifdef ARCH_PLUGIN_DRIVER
#error "More than one PluginDriver included!"
#endif
#define ARCH_PLUGIN_DRIVER PluginDriver_Win32 

#endif

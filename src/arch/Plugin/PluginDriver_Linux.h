#ifndef PluginDriver_Linux_H
#define PluginDriver_Linux_H

#include "PluginDriver.h"

class LoadedPluginLinux : public LoadedPluginLibrary {
public:
	LoadedPluginLinux(RString libraryPath);
};

class PluginDriver_Linux : public PluginDriver
{
public:
	virtual void GetAvailablePlugins(std::vector<LoadedPlugin*>& out);
};

#ifdef ARCH_PLUGIN_DRIVER
#error "More than one PluginDriver included!"
#endif
#define ARCH_PLUGIN_DRIVER PluginDriver_Linux

#endif

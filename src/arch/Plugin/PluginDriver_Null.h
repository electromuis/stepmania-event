#ifndef PluginDriver_Null_H
#define PluginDriver_Null_H

#include "PluginDriver.h"
#include <vector>

class PluginDriver_Null : public PluginDriver
{
public:
	virtual void GetAvailablePlugins(std::vector<LoadedPlugin*>& out);
};

#endif

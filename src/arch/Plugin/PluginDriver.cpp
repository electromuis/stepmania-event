#include "PluginDriver.h"

#include "global.h"
#include "PluginDriver_Null.h"
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

void PluginDriver::GetAvailablePlugins(std::vector<LoadedPlugin*>& out)
{
	auto list = LoadedPluginEmbedded::GetList();
	for (LoadedPluginEmbedded* p : list)
	{
		out.push_back(p);
	}
}

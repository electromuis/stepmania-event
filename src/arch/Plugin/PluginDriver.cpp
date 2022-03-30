#include "PluginDriver.h"

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

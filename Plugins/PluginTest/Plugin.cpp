#pragma once

#include "Plugin.h"
#include "RageLog.h"

REGISTER_PLUGIN(TestPlugin, "Test Plugin", "0.1.1")

PluginTest::PluginTest()
{
	LOG->Info("Loaded the actual plugintest :o");
}

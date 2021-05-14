#pragma once

#include "global.h"
#include "Plugin.h"
#include "RageLog.h"

REGISTER_PLUGIN(PluginTest, "Test Plugin", "0.1.1")

PluginTest::PluginTest()
{
	LOG->Info("Loaded the actual plugintest :o");
}

void PluginTest::Update(float fDeltaTime)
{

}

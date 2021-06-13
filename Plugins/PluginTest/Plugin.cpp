#pragma once

#include "global.h"
#include "Plugin.h"

REGISTER_PLUGIN(PluginTest, "Test Plugin", "0.1.1")

PluginTest::PluginTest()
{

}

void PluginTest::Update(float fDeltaTime)
{

}

void PluginTest::Start(StepmaniaPointer* stepmaniaPointer)
{
	sm = stepmaniaPointer;

	sm->log->Info("Loaded the actual plugintest :o");
}

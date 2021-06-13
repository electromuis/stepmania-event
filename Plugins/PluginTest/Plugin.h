#pragma once

#include "config.hpp"
#include "global.h"
#include "PluginManager.h"



class PluginTest : public PluginBase {
public:
	PluginTest();

	virtual void Update(float fDeltaTime);
	void Start(StepmaniaPointer* stepmaniaPointer);
};

#pragma once

#include "ScreenOptions.h"

class ScreenPadmiss : public ScreenOptions
{
public:
	ScreenPadmiss();
	virtual ~ScreenPadmiss();

	virtual void Init();
	virtual void BeginScreen();
	virtual void HandleScreenMessage(const ScreenMessage SM);

protected:
	vector<OptionRowHandler*> OptionRowHandlers;

	virtual void ProcessMenuStart(const InputEventPlus& input);

	virtual void ImportOptions(int iRow, const vector<PlayerNumber>& vpns);
	virtual void ExportOptions(int iRow, const vector<PlayerNumber>& vpns);

};

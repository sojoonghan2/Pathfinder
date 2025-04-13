#pragma once
#include "MasterScript.h"

class CrashScript : public MasterScript
{
public:
	CrashScript();
	virtual ~CrashScript();

	virtual void LateUpdate() override;

private:
};

#pragma once
#include "MasterScript.h"

class ExplorationScript : public MasterScript
{
public:
	ExplorationScript();
	virtual ~ExplorationScript();

	virtual void LateUpdate() override;

private:
};

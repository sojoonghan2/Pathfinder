#pragma once
#include "MasterScript.h"

class FactoryScript : public MasterScript
{
public:
	FactoryScript();
	virtual ~FactoryScript();

	virtual void LateUpdate() override;

private:
};

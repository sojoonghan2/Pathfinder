#pragma once
#include "MasterScript.h"

class FactoryScript : public MasterScript
{
public:
	FactoryScript();
	virtual ~FactoryScript();

	virtual void Start() override;
	virtual void LateUpdate() override;

private:
};

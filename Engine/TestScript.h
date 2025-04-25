#pragma once
#include "MasterScript.h"

class TestScript : public MasterScript
{
public:
	TestScript();
	virtual ~TestScript();

	virtual void LateUpdate() override;

private:
};

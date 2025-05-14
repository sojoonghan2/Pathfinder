#pragma once
#include "MasterScript.h"

class BossScript : public MasterScript
{
public:
	BossScript();
	virtual ~BossScript();

	virtual void LateUpdate() override;

private:
};

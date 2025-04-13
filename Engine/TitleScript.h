#pragma once
#include "MasterScript.h"

class TitleScript : public MasterScript
{
public:
	TitleScript();
	virtual ~TitleScript();

	virtual void LateUpdate() override;

private:
};

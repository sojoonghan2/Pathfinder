#pragma once
#include "MasterScript.h"

class LuckyScript : public MasterScript
{
public:
    LuckyScript();
    virtual ~LuckyScript();

    virtual void LateUpdate() override;

private:
};

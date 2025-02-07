#pragma once
#include "MasterScript.h"

class RuinsScript : public MasterScript
{
public:
    RuinsScript();
    virtual ~RuinsScript();

    virtual void LateUpdate() override;

private:
};

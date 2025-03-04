#pragma once
#include "MasterScript.h"

class RuinsScript : public MasterScript
{
public:
    RuinsScript();
    virtual ~RuinsScript();

    virtual void LateUpdate() override;

    void Occupation();

private:
    shared_ptr<GameObject>  _water;
    shared_ptr<GameObject>  _player;
};

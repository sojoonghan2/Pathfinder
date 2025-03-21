#pragma once
#include "MasterScript.h"

class LuckyScript : public MasterScript
{
public:
    LuckyScript();
    virtual ~LuckyScript();

    virtual void LateUpdate() override;

    void KeyboardInput();
    void MouseInput();

private:
    shared_ptr<GameObject>  _treasureChest;

    bool                    _getChest = false;
};

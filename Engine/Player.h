#pragma once
#include "Component.h"

class Player : public Component
{
public:
    Player();
    virtual ~Player();

    virtual void FinalUpdate() override;

    void LoadPlayerSkill();
    void LoadGrenade();

public:
    bool        _isInit = false;
};

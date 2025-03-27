#pragma once
#include "Component.h"

class Player : public Component
{
public:
    Player();
    virtual ~Player();

    virtual void FinalUpdate() override;

    void LoadPlayerSkill();
    void Init();

public:
    bool        _isInit = false;
};

#pragma once
#include "MonoBehaviour.h"

class TestGrenadeScript : public MonoBehaviour
{
public:
    TestGrenadeScript();
    virtual ~TestGrenadeScript();

    virtual void LateUpdate() override;

    void KeyboardInput();
    void MouseInput();

    void ThrowGrenade();

private:
    bool _isThrown = false;
    bool _resetThrow = false;

    Vec3 _velocity;
    float _gravity;

    float _angle = 45.0f;
    float _power = 1000.0f;
};

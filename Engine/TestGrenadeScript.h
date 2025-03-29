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
    float _power = 2000.0f;

    float _timeSinceLanded;

    bool _pendingThrow = false;
    float _throwDelay = 0.0f;

    // 부모 트랜스폼 캐시
    shared_ptr<Transform> _parentTransform;
    float _grenadeCooldown = 10.f;
    float _grenadeCooldownTimer = 0.0f;

};

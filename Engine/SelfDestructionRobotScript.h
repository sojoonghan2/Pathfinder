#pragma once
#include "MonoBehaviour.h"

class SelfDestructionRobotScript : public MonoBehaviour
{
public:
    SelfDestructionRobotScript();
    virtual ~SelfDestructionRobotScript();

    virtual void LateUpdate() override;

private:
    void MoveRandomly();
    void CheckBoundary();

private:
    float _speed = 1000.f;
    Vec3 _direction;
    float _changeDirectionTime = 10.0f;
    float _elapsedTime = 0.0f;
};
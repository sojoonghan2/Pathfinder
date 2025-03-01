#include "pch.h"
#include "SelfDestructionRobotScript.h"
#include "Input.h"
#include "Engine.h"
#include "Timer.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "ParticleSystem.h"
#include "Transform.h"

SelfDestructionRobotScript::SelfDestructionRobotScript()
{
    srand(static_cast<unsigned int>(time(nullptr))); // ���� �õ� ����
    _direction.x = (rand() % 2 == 0) ? 1.0f : -1.0f;
    _direction.z = (rand() % 2 == 0) ? 1.0f : -1.0f;
    _direction.y = 0.0f;
}

SelfDestructionRobotScript::~SelfDestructionRobotScript() {}

void SelfDestructionRobotScript::LateUpdate()
{
    MoveRandomly();
    CheckBoundary();
}

void SelfDestructionRobotScript::MoveRandomly()
{
    _elapsedTime += DELTA_TIME;

    // ���� �ð��� ������ ���� ����
    if (_elapsedTime >= _changeDirectionTime)
    {
        _elapsedTime = 0.0f;
        _direction.x = (rand() % 2 == 0) ? 1.0f : -1.0f;
        _direction.z = (rand() % 2 == 0) ? 1.0f : -1.0f;
    }

    Vec3 pos = GetTransform()->GetLocalPosition();
    pos += _direction * _speed * DELTA_TIME;
    GetTransform()->SetLocalPosition(pos);
}

void SelfDestructionRobotScript::CheckBoundary()
{
    Vec3 pos = GetTransform()->GetLocalPosition();

    // �� ��踦 ������ �ݴ� �������� �̵��ϵ��� ����
    float mapMinX = -4900.f;
    float mapMaxX = 4900.f;
    float mapMinZ = -4900.f;
    float mapMaxZ = 4900.f;
    float minY = 500.f;
    float maxY = 9500.f;

    if (pos.x <= mapMinX || pos.x >= mapMaxX)
    {
        _direction.x *= -1;  // X ���� ����
        pos.x = max(mapMinX, min(pos.x, mapMaxX));
    }

    if (pos.z <= mapMinZ || pos.z >= mapMaxZ)
    {
        _direction.z *= -1;  // Z ���� ����
        pos.z = max(mapMinZ, min(pos.z, mapMaxZ));
    }

    pos.y = max(minY, min(pos.y, maxY));  // Y ���� ���� ���� ����

    GetTransform()->SetLocalPosition(pos);
}
